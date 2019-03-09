#include "PullPointSubscription.h"

#include <chrono>
#include <thread>
#include <future>

#include <gSoap/wsaapi.h>

#include "AuthorisationHolder.h"


namespace Onvif
{

	timeval* convertTime(soap* soap, std::time_t time)
	{
		timeval* tm = soap_new_xsd__dateTime(soap);

		tm->tv_sec = static_cast<long>(time / 1000000);
		tm->tv_usec = static_cast<long>(time % 1000000);

		return tm;
	}

	

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

	auto cur_tssTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	tt_msg->UtcTime = *convertTime(soap, cur_tssTime);

	tt_msg->PropertyOperation = soap_new_tt__PropertyOperation(soap);
	*tt_msg->PropertyOperation = propOperation;

	tt_msg->Source = soap_new_tt__ItemList(soap);

	_tt__ItemList_SimpleItem prop_vToken;
	prop_vToken.Name = "Id";
	prop_vToken.Value = "asd-13sd";
	tt_msg->Source->SimpleItem.push_back(prop_vToken);

	tt_msg->Data = soap_new_tt__ItemList(soap);

	_tt__ItemList_SimpleItem propPicture;
	propPicture.Name = "Picture";
	propPicture.Value = "AAAAAAAAAAAAAA==";
	tt_msg->Data->SimpleItem.push_back(propPicture);

	_tt__ItemList_SimpleItem propAccount;
	propAccount.Name = "Account";
	propAccount.Value = "operator";
	tt_msg->Data->SimpleItem.push_back(propAccount);


	msg->Message.__any.set(tt_msg, SOAP_TYPE__tt__Message);

	return msg;
}

int PullPointSubscription::PullMessages(_tev__PullMessages *tev__PullMessages, _tev__PullMessagesResponse &tev__PullMessagesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}


	auto cur_tssTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
	auto termTime = cur_tssTime + std::chrono::duration_cast<std::chrono::microseconds>(tev__PullMessages->Timeout);

	std::async(std::launch::async,
		[&]()
	{
		std::this_thread::sleep_until(std::chrono::system_clock::time_point(termTime));

		tev__PullMessagesResponse.CurrentTime = *convertTime(this->soap, cur_tssTime.count());
		tev__PullMessagesResponse.TerminationTime = *convertTime(this->soap, cur_tssTime.count() + 60000000);

		auto message = CreateMetallDetectorEvent(soap, tt__PropertyOperation::Initialized);

		message->SubscriptionReference = soap_new_wsa5__EndpointReferenceType(this->soap);
		message->SubscriptionReference->Address = soap_strdup(this->soap, this->soap->endpoint);

		tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(message);

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