#include "SubscriptionManager.h"

#include <gSoap/wsaapi.h>

#include "AuthorisationHolder.h"


namespace Onvif
{

SubscriptionManager::SubscriptionManager(struct soap *_soap):
	SubscriptionManagerBindingService(_soap)
{
}


int SubscriptionManager::Renew(_wsnt__Renew *wsnt__Renew, _wsnt__RenewResponse &wsnt__RenewResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	return soap_wsa_reply(this->soap, nullptr, "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/RenewResponse");
}

int SubscriptionManager::Unsubscribe(_wsnt__Unsubscribe *wsnt__Unsubscribe, _wsnt__UnsubscribeResponse &wsnt__UnsubscribeResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}


	return soap_wsa_reply(this->soap, nullptr, "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/UnsubscribeResponse");
}

}