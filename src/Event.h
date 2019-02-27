#ifndef ONVIF_EVENT_H
#define ONVIF_EVENT_H

#include <gSoap/soapEventBindingService.h>

namespace Onvif
{

class Event : public EventBindingService
{
public:
	Event(struct soap *_soap);

	bool verifyPasswordDA();


	int GetEventProperties(_tev__GetEventProperties *tev__GetEventProperties, _tev__GetEventPropertiesResponse &tev__GetEventPropertiesResponse) override;
};
}

#endif

