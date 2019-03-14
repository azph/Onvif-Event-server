#include "OnvifServer.h"

#include <list>
#include <algorithm>

#include <gSoap/httpda.h>
#include <gSoap/wsaapi.h>

#include "Device.h"
#include "Event.h"
#include "PullPointSubscription.h"
#include "SubscriptionManager.h"
#include "SubscriptionController.h"

namespace Onvif
{

const int MAX_THREADS_COUNT = 10;

Onvif::OnvifServer& OnvifServer::getInstance()
{
	static OnvifServer server;

	return server;
}

void OnvifServer::start()
{
	m_serverTread = std::thread([&]() { onStartServices(); });
}

void OnvifServer::stop()
{
	m_serverTread.join();
}

OnvifServer::OnvifServer()
{

}

OnvifServer::~OnvifServer()
{

}

void OnvifServer::onStartServices()
{
	struct soap *soap = soap_new1(SOAP_XML_STRICT | SOAP_XML_CANONICAL | SOAP_C_UTFSTRING);
	//struct soap *soap = soap_new();
	soap_register_plugin_arg(soap, http_da, http_da_md5());
	soap_register_plugin(soap, soap_wsa);

	if (!soap_valid_socket(soap_bind(soap, NULL, 8080, 100)))
		exit(EXIT_FAILURE);
	soap->max_keep_alive = 0;

	std::list<std::future<void> > futureList;

	auto controller = std::make_shared<SubscriptionController>();
	
	while (1)
	{
		if (!soap_valid_socket(soap_accept(soap)))
			exit(EXIT_FAILURE);
		
		auto soapCopy = soap_copy(soap);

		auto device = std::make_shared<Onvif::Device>(soapCopy);
		auto event = std::make_shared<Onvif::Event>(soapCopy, controller);
		auto pullPointSubscription = std::make_shared<Onvif::PullPointSubscription>(soapCopy, controller);
		auto subscriptionManager = std::make_shared<Onvif::SubscriptionManager>(soapCopy, controller);

		while (futureList.size() >= MAX_THREADS_COUNT)
		{
			const auto& it = std::find_if(futureList.begin(), futureList.end(), 
				[](const std::future<void>& f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; });
			
			if (it != futureList.end())
			{
				it->wait();
				futureList.erase(it);
				continue;
			}
			futureList.front().wait_for(std::chrono::milliseconds(500));
		}

		auto f = std::async([=]()
		{
			int err = 0;

			if (soap_begin_serve(soapCopy))
			{
				soap_stream_fault(soapCopy, std::cerr);
			}
			else if ((err = device->dispatch()) == SOAP_NO_METHOD)
			{
				if ((err = event->dispatch()) == SOAP_NO_METHOD)
				{
					if ((err =  subscriptionManager->dispatch()) == SOAP_NO_METHOD)
					{
						err = pullPointSubscription->dispatch();
					}
				}
			}

			if (err)
			{
				soap_send_fault(soapCopy);
			}

			soap_destroy(soapCopy);
			soap_end(soapCopy);
			soap_free(soapCopy);
		});

		futureList.push_back(std::move(f));
		
	}

	soap_free(soap); // safe to delete when abc, uvw, xyz are also deleted
}

}