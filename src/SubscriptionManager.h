#ifndef ONVIF_SUBSCRIPTION_MANAGER_H
#define ONVIF_SUBSCRIPTION_MANAGER_H

#include <gSoap/soapSubscriptionManagerBindingService.h>

namespace Onvif
{

class SubscriptionManager : public SubscriptionManagerBindingService
{
public:
	SubscriptionManager(struct soap *_soap);

	int Renew(_wsnt__Renew *wsnt__Renew, _wsnt__RenewResponse &wsnt__RenewResponse) override;
	int Unsubscribe(_wsnt__Unsubscribe *wsnt__Unsubscribe, _wsnt__UnsubscribeResponse &wsnt__UnsubscribeResponse) override;
};
}

#endif

