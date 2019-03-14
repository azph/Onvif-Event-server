#include "PullPointSubscription.h"

#include <chrono>
#include <thread>
#include <future>

#include <gSoap/wsaapi.h>

#include "AuthorisationHolder.h"
#include "SoapHelpers.h"

namespace Onvif
{

PullPointSubscription::PullPointSubscription(struct soap *_soap, SubscriptionControllerSP controller):
	PullPointSubscriptionBindingService(_soap),
	m_controller(controller)
{
}

wsnt__NotificationMessageHolderType* CreateMetallDetectorEvent(struct soap* soap, tt__PropertyOperation propOperation)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeCustom(soap);
	msg->Topic->__any = "tns1:Device/tmk:MetalDetector/tmk:Detect";
	msg->Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";

	_tt__Message * tt_msg = soap_new__tt__Message(soap);

	auto cur_tssTime = SoapHelpers::getCurrentTime();

	tt_msg->UtcTime = *SoapHelpers::convertTime(soap, cur_tssTime);

	tt_msg->PropertyOperation = soap_new_tt__PropertyOperation(soap);
	*tt_msg->PropertyOperation = propOperation;

	tt_msg->Source = soap_new_tt__ItemList(soap);

	tt_msg->Source->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Id", "asd-13sd"));

	tt_msg->Data = soap_new_tt__ItemList(soap);
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Picture", "AAAAAAAAAAAAAA=="));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Account", "operator"));

	msg->Message.__any.set(tt_msg, SOAP_TYPE__tt__Message);
	msg->SubscriptionReference = soap_new_wsa5__EndpointReferenceType(soap);
	msg->SubscriptionReference->Address = soap_strdup(soap, soap->endpoint);

	return msg;
}

wsnt__NotificationMessageHolderType* CreateSteamDetectorEvent(struct soap* soap, tt__PropertyOperation propOperation, const std::string& mesures)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeCustom(soap);
	msg->Topic->__any = "tns1:Device/tmk:SteamDetector/tmk:Detect";
	msg->Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";

	_tt__Message * tt_msg = soap_new__tt__Message(soap);

	auto cur_tssTime = SoapHelpers::getCurrentTime();

	tt_msg->UtcTime = *SoapHelpers::convertTime(soap, cur_tssTime);

	tt_msg->PropertyOperation = soap_new_tt__PropertyOperation(soap);
	*tt_msg->PropertyOperation = propOperation;

	tt_msg->Source = soap_new_tt__ItemList(soap);
	tt_msg->Source->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Id", "asd-13sd"));

	tt_msg->Data = soap_new_tt__ItemList(soap);
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Picture", "AAAAAAAAAAAAAA=="));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Account", "operator"));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Mesures", mesures.c_str()));

	msg->Message.__any.set(tt_msg, SOAP_TYPE__tt__Message);
	msg->SubscriptionReference = soap_new_wsa5__EndpointReferenceType(soap);
	msg->SubscriptionReference->Address = soap_strdup(soap, soap->endpoint);

	return msg;
}

wsnt__NotificationMessageHolderType* CreateRadiationMonitoringEvent(struct soap* soap, tt__PropertyOperation propOperation, const std::string& mesures)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeCustom(soap);
	msg->Topic->__any = "tns1:Device/tmk:RadiationMonitoring/tmk:Detect";
	msg->Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";

	_tt__Message * tt_msg = soap_new__tt__Message(soap);

	auto cur_tssTime = SoapHelpers::getCurrentTime();

	tt_msg->UtcTime = *SoapHelpers::convertTime(soap, cur_tssTime);

	tt_msg->PropertyOperation = soap_new_tt__PropertyOperation(soap);
	*tt_msg->PropertyOperation = propOperation;

	tt_msg->Source = soap_new_tt__ItemList(soap);

	tt_msg->Source = soap_new_tt__ItemList(soap);
	tt_msg->Source->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Id", "asd-13sd"));

	tt_msg->Data = soap_new_tt__ItemList(soap);
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Picture", "AAAAAAAAAAAAAA=="));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Category", "Dangerous"));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Account", "operator"));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Mesures", mesures.c_str()));

	msg->Message.__any.set(tt_msg, SOAP_TYPE__tt__Message);
	msg->SubscriptionReference = soap_new_wsa5__EndpointReferenceType(soap);
	msg->SubscriptionReference->Address = soap_strdup(soap, soap->endpoint);

	return msg;
}

int PullPointSubscription::PullMessages(_tev__PullMessages *tev__PullMessages, _tev__PullMessagesResponse &tev__PullMessagesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	std::string uuid = SoapHelpers::getUuidFromHost(soap->path, "sub=");
	if (uuid.empty())
	{
		return soap_wsa_sender_fault(soap, "Invalid URI.", NULL);
	}

	auto eventSubscription = m_controller->getSubscription(uuid);

	auto cur_tssTime = SoapHelpers::getCurrentTime();

	if (!eventSubscription || eventSubscription->getTermTime().count() < cur_tssTime)
	{
		return soap_wsa_sender_fault(soap, "Invalid Subscription id", NULL);
	}


	auto waitFor =  std::chrono::duration_cast<std::chrono::milliseconds>(tev__PullMessages->Timeout);

	eventSubscription->setTermTime(std::chrono::milliseconds(cur_tssTime) + waitFor + DEFAULT_KEEP_ALIVE_TIMEOUT);

	auto messages = eventSubscription->getMessages(waitFor, tev__PullMessages->MessageLimit);

	cur_tssTime = SoapHelpers::getCurrentTime();

	tev__PullMessagesResponse.CurrentTime = *SoapHelpers::convertTime(this->soap, cur_tssTime);
	eventSubscription->setTermTime(std::chrono::milliseconds(cur_tssTime) + DEFAULT_KEEP_ALIVE_TIMEOUT);
	tev__PullMessagesResponse.TerminationTime = *SoapHelpers::convertTime(this->soap, eventSubscription->getTermTime().count());

	for (const auto& value : messages)
	{
		switch (value.type)
		{
		case MessageType::MetallDetector :
			tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateMetallDetectorEvent(soap, tt__PropertyOperation::Initialized));
			break;
		case MessageType::SteamDetector:
			tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateSteamDetectorEvent(soap, tt__PropertyOperation::Initialized, value.Mesures));
			break;
		case MessageType::RadiationMonitoring:
			tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateRadiationMonitoringEvent(soap, tt__PropertyOperation::Initialized, value.Mesures));
			break;
		}
	}


	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/PullMessagesResponse");
}

int PullPointSubscription::SetSynchronizationPoint(_tev__SetSynchronizationPoint *tev__SetSynchronizationPoint, _tev__SetSynchronizationPointResponse &tev__SetSynchronizationPointResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/SetSynchronizationPointResponse");
}

int PullPointSubscription::Unsubscribe(_wsnt__Unsubscribe *wsnt__Unsubscribe, _wsnt__UnsubscribeResponse &wsnt__UnsubscribeResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/SetSynchronizationPointResponse");
}

}