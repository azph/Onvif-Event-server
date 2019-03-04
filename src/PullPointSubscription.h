#ifndef ONVIF_PULLPOINT_SUBSCRIPTION_H
#define ONVIF_PULLPOINT_SUBSCRIPTION_H

#include <gSoap/soapPullPointSubscriptionBindingService.h>

namespace Onvif
{

class PullPointSubscription : public PullPointSubscriptionBindingService
{
public:
	PullPointSubscription(struct soap *_soap);

	int PullMessages(_tev__PullMessages *tev__PullMessages, _tev__PullMessagesResponse &tev__PullMessagesResponse) override;
	int SetSynchronizationPoint(_tev__SetSynchronizationPoint *tev__SetSynchronizationPoint, _tev__SetSynchronizationPointResponse &tev__SetSynchronizationPointResponse) override;
};
}

#endif

