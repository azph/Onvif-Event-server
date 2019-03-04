#include "Event.h"

#include <gSoap/wsaapi.h>

#include "AuthorisationHolder.h"


namespace Onvif
{
soap_dom_element createMetalDetectorDescription(struct soap* soap);

Event::Event(struct soap *_soap):
	EventBindingService(_soap)
{
}



int Event::CreatePullPointSubscription(_tev__CreatePullPointSubscription *tev__CreatePullPointSubscription, _tev__CreatePullPointSubscriptionResponse &tev__CreatePullPointSubscriptionResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	std::time_t lt = std::time(nullptr) * 1000;
	std::string str = soap->endpoint + std::string("?sub=1");
	tev__CreatePullPointSubscriptionResponse.SubscriptionReference.Address = soap_strdup(soap, str.c_str());

	tev__CreatePullPointSubscriptionResponse.wsnt__CurrentTime.tv_sec = lt / 1000000;
	tev__CreatePullPointSubscriptionResponse.wsnt__CurrentTime.tv_usec = lt % 1000000;

	tev__CreatePullPointSubscriptionResponse.wsnt__CurrentTime.tv_sec = lt / 1000000 + 60;
	tev__CreatePullPointSubscriptionResponse.wsnt__CurrentTime.tv_usec = lt % 1000000;
	
	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/EventPortType/CreatePullPointSubscriptionResponse");
}

static soap_dom_element AddHolder(struct soap* soap, soap_dom_element& child, const char* holderName, bool isTopic)
{
	auto holder = soap_new_tt__AnyHolder(soap);
	holder->__any.push_back(child);

	if (isTopic)
	{
		holder->__anyAttribute.name = "wstop:topic";
		holder->__anyAttribute.text = soap_bool2s(soap, true);
	}

	soap_dom_element holderEl(soap);
	holderEl.set(holder, SOAP_TYPE_tt__AnyHolder);
	holderEl.name = holderName;

	return holderEl;
}

int Event::GetEventProperties(_tev__GetEventProperties *tev__GetEventProperties, _tev__GetEventPropertiesResponse &tev__GetEventPropertiesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	tev__GetEventPropertiesResponse.TopicNamespaceLocation.push_back("http://www.onvif.org/onvif/ver10/topics/topicns.xml");
	tev__GetEventPropertiesResponse.wsnt__FixedTopicSet = true;
	tev__GetEventPropertiesResponse.wsnt__TopicExpressionDialect.push_back("http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
	tev__GetEventPropertiesResponse.wsnt__TopicExpressionDialect.push_back("http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete");
	tev__GetEventPropertiesResponse.MessageContentFilterDialect.push_back("http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter");
	tev__GetEventPropertiesResponse.MessageContentSchemaLocation.push_back("http://www.onvif.org/onvif/ver10/schema/onvif.xsd");

	tev__GetEventPropertiesResponse.wstop__TopicSet = tev__GetEventPropertiesResponse.wstop__TopicSet = soap_new_wstop__TopicSetType(soap);

	tev__GetEventPropertiesResponse.wstop__TopicSet->__any.push_back(createMetalDetectorDescription(soap));
	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/EventPortType/GetEventPropertiesResponse");
}

soap_dom_element createMetalDetectorDescription(struct soap* soap)
{
	tt__MessageDescription * _MessageDescription_RelayEvt = soap_new_tt__MessageDescription(soap);

	_MessageDescription_RelayEvt->Source = soap_new_tt__ItemListDescription(soap);

	_tt__ItemListDescription_SimpleItemDescription sourceDescr;
	sourceDescr.Name = "Id";
	sourceDescr.Type = "xsd:string";
	_MessageDescription_RelayEvt->Source->SimpleItemDescription.push_back(sourceDescr);

	_MessageDescription_RelayEvt->Data = soap_new_tt__ItemListDescription(soap);

	_tt__ItemListDescription_SimpleItemDescription PictureDescr;
	PictureDescr.Name = "Picture";
	PictureDescr.Type = "xsd:string";
	_MessageDescription_RelayEvt->Data->SimpleItemDescription.push_back(PictureDescr);

	_tt__ItemListDescription_SimpleItemDescription elemDescr;
	elemDescr.Name = "Account";
	elemDescr.Type = "xsd:string";
	_MessageDescription_RelayEvt->Data->SimpleItemDescription.push_back(elemDescr);

	soap_dom_element msgDescr(soap);
	msgDescr.set(_MessageDescription_RelayEvt, SOAP_TYPE_tt__MessageDescription);
	msgDescr.name = "tt:MessageDescription";

	soap_dom_element relayEvtDescrRelayEl = AddHolder(soap, msgDescr, "tmk:Detect", true);
	soap_dom_element relayEvtDescrTriggerEl = AddHolder(soap, relayEvtDescrRelayEl, "tmk:MetalDetector", false);
	soap_dom_element relayEvtDescrDeviceEl = AddHolder(soap, relayEvtDescrTriggerEl, "tns1:Device", false);

	return relayEvtDescrDeviceEl;

}
}