#include "PullPointSubscription.h"

#include <chrono>
#include <thread>
#include <future>

#include <gSoap/wsaapi.h>

#include "AuthorisationHolder.h"
#include "SoapHelpers.h"

namespace Onvif
{

PullPointSubscription::PullPointSubscription(struct soap *_soap):
	PullPointSubscriptionBindingService(_soap)
{
}

wsnt__NotificationMessageHolderType* CreateMetallDetectorEvent(struct soap* soap, tt__PropertyOperation propOperation)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeAxxon(soap);
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

wsnt__NotificationMessageHolderType* CreateSteamDetectorEvent(struct soap* soap, tt__PropertyOperation propOperation)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeAxxon(soap);
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
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Mesures",
		"TYPE1, INTENS1, POS1, AMPL1, TYPE2, NTENS2, POS2, AMPL2, TYPE3, INTENS3, POS3, AMPL3, TYPE4, INTENS4, POS4, AMPL4"));

	msg->Message.__any.set(tt_msg, SOAP_TYPE__tt__Message);
	msg->SubscriptionReference = soap_new_wsa5__EndpointReferenceType(soap);
	msg->SubscriptionReference->Address = soap_strdup(soap, soap->endpoint);

	return msg;
}

wsnt__NotificationMessageHolderType* CreateRadiationMonitoringEvent(struct soap* soap, tt__PropertyOperation propOperation)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeAxxon(soap);
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
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Mesures", "0xRadSignal"));

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


	auto cur_tssTime = SoapHelpers::getCurrentTime();
	auto termTime = cur_tssTime + std::chrono::duration_cast<std::chrono::microseconds>(tev__PullMessages->Timeout).count();

	std::async(std::launch::async,
		[&]()
	{
		std::this_thread::sleep_until(std::chrono::system_clock::from_time_t(cur_tssTime + 50000));

		tev__PullMessagesResponse.CurrentTime = *SoapHelpers::convertTime(this->soap, cur_tssTime);
		tev__PullMessagesResponse.TerminationTime = *SoapHelpers::convertTime(this->soap, termTime + 30000000);

		tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateMetallDetectorEvent(soap, tt__PropertyOperation::Initialized));
		tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateSteamDetectorEvent(soap, tt__PropertyOperation::Initialized));
		tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateRadiationMonitoringEvent(soap, tt__PropertyOperation::Initialized));

	}).get();

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

}