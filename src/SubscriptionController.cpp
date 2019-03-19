#include "SubscriptionController.h"

namespace Onvif
{
const std::chrono::milliseconds DEFAULT_KEEP_ALIVE_TIMEOUT(60000);

EventSubscription::EventSubscription(FilterType filter) :
	m_termTime(SoapHelpers::getCurrentTime()),
	m_atomic(ATOMIC_VAR_INIT(0)),
	m_filter(filter)
{

}

std::chrono::milliseconds EventSubscription::getTermTime()
{
	return m_termTime;
}

void EventSubscription::setTermTime(std::chrono::milliseconds termTime)
{
	m_termTime = termTime;
}

std::vector<NotificationMessage> EventSubscription::getMessages(std::chrono::milliseconds waitFor, std::uint32_t maxMesages)
{
	std::vector<NotificationMessage> result;

	std::unique_lock<std::mutex> lk(m_mutex);
	if (m_messages.empty())
	{
		if (!m_cv.wait_for(lk, waitFor, [this]() {return m_atomic == 1; }))
		{
			return result;
		}
	}

	for (std::uint32_t i = 0; i < m_messages.size() && i < maxMesages; ++i)
	{
		result.push_back(m_messages.front());
		m_messages.pop();
	}

	return result;
}

void EventSubscription::sendMessage(const NotificationMessage& message)
{
	std::unique_lock<std::mutex> lk(m_mutex);
	
	if (m_filter && std::find(m_filter->begin(), m_filter->end(), message.type) == m_filter->end())
	{
		return;
	}

	m_messages.push(message);
	m_atomic = 1;
	m_cv.notify_all();
}

SubscriptionController::SubscriptionController() :
	m_active(false)
{

}

SubscriptionController::~SubscriptionController()
{

}

EventSubscriptionSP SubscriptionController::createSubscription(const std::string& uuid, FilterType filter)
{
	auto subscription = std::make_shared<EventSubscription>(filter);
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_subscriptions[uuid] = subscription;
	}
	if (m_active == false)
	{
		startReadEvents();
	}

	return subscription;
}

bool SubscriptionController::removeSubscription(const std::string& uuid)
{
	bool result = false;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		result = m_subscriptions.erase(uuid) != 0;

		if (m_subscriptions.empty() && m_active == true)
		{
			lock.unlock();
			stopReadEvents();
		}
	}

	return result;
}

Onvif::EventSubscriptionSP SubscriptionController::getSubscription(const std::string& uuid) const
{
	auto it = m_subscriptions.find(uuid);
	if (it != m_subscriptions.end())
	{
		return it->second;
	}
	return EventSubscriptionSP();
}

void SubscriptionController::startReadEvents()
{
	m_active = true;
	m_eventTreadFuture = std::async([&]()
	{
		while (m_active)
		{
			//auto curTime = SoapHelpers::getCurrentTime();
			//NotificationMessage messageMetall = { MessageType::MetallDetector, "", std::chrono::milliseconds(curTime) };
			///NotificationMessage messageRad = { MessageType::RadiationMonitoring, "More Than 0,3 mPT", std::chrono::milliseconds(curTime) };

			auto messages = m_reader.ReadEvents(); 

			{
				std::unique_lock<std::mutex> lock(m_mutex);
				for (auto& value : m_subscriptions)
				{
					for(const auto & mes : messages)
					{
						value.second->sendMessage(mes);	
					}
			
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	});
}

void SubscriptionController::stopReadEvents()
{
	m_active = false;
	m_eventTreadFuture.get();
}

}