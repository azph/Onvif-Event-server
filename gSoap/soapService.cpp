/* soapService.cpp
   Generated by gSOAP 2.8.78 for WS/onvifFull.h

gSOAP XML Web services tools
Copyright (C) 2000-2018, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#include "soapService.h"

Service::Service()
{	this->soap = soap_new();
	this->soap_own = true;
	Service_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

Service::Service(const Service& rhs)
{	this->soap = rhs.soap;
	this->soap_own = false;
}

Service::Service(struct soap *_soap)
{	this->soap = _soap;
	this->soap_own = false;
	Service_init(_soap->imode, _soap->omode);
}

Service::Service(soap_mode iomode)
{	this->soap = soap_new();
	this->soap_own = true;
	Service_init(iomode, iomode);
}

Service::Service(soap_mode imode, soap_mode omode)
{	this->soap = soap_new();
	this->soap_own = true;
	Service_init(imode, omode);
}

Service::~Service()
{	if (this->soap_own)
	{	this->destroy();
		soap_free(this->soap);
	}
}

void Service::Service_init(soap_mode imode, soap_mode omode)
{	soap_imode(this->soap, imode);
	soap_omode(this->soap, omode);
	static const struct Namespace namespaces[] = {
        { "SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL },
        { "SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL },
        { "xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL },
        { "xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL },
        { "chan", "http://schemas.microsoft.com/ws/2005/02/duplex", NULL, NULL },
        { "wsa5", "http://www.w3.org/2005/08/addressing", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL },
        { "tmk", "http://www.inforion.ru/schemas/sstmk/onvif/topics/sensors", NULL, NULL },
        { "xmime", "http://www.w3.org/2005/05/xmlmime", NULL, NULL },
        { "tt", "http://www.onvif.org/ver10/schema", NULL, NULL },
        { "xop", "http://www.w3.org/2004/08/xop/include", NULL, NULL },
        { "wsrfbf", "http://docs.oasis-open.org/wsrf/bf-2", NULL, NULL },
        { "wstop", "http://docs.oasis-open.org/wsn/t-1", NULL, NULL },
        { "wsrf", "http://docs.oasis-open.org/wsrf/r-2", NULL, NULL },
        { "tds", "http://www.onvif.org/ver10/device/wsdl", NULL, NULL },
        { "tevcppb", "http://www.onvif.org/ver10/events/wsdl/CreatePullPointBinding", NULL, NULL },
        { "teveb", "http://www.onvif.org/ver10/events/wsdl/EventBinding", NULL, NULL },
        { "tevncb", "http://www.onvif.org/ver10/events/wsdl/NotificationConsumerBinding", NULL, NULL },
        { "tevnpb", "http://www.onvif.org/ver10/events/wsdl/NotificationProducerBinding", NULL, NULL },
        { "tevppb", "http://www.onvif.org/ver10/events/wsdl/PullPointBinding", NULL, NULL },
        { "tev", "http://www.onvif.org/ver10/events/wsdl", NULL, NULL },
        { "tevpps", "http://www.onvif.org/ver10/events/wsdl/PullPointSubscriptionBinding", NULL, NULL },
        { "tevpsmb", "http://www.onvif.org/ver10/events/wsdl/PausableSubscriptionManagerBinding", NULL, NULL },
        { "wsnt", "http://docs.oasis-open.org/wsn/b-2", NULL, NULL },
        { "tevsmb", "http://www.onvif.org/ver10/events/wsdl/SubscriptionManagerBinding", NULL, NULL },
        { "wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL, NULL },
        { "wsd", "http://schemas.xmlsoap.org/ws/2005/04/discovery", NULL, NULL },
        { "c14n", "http://www.w3.org/2001/10/xml-exc-c14n#", NULL, NULL },
        { "ds", "http://www.w3.org/2000/09/xmldsig#", NULL, NULL },
        { "saml1", "urn:oasis:names:tc:SAML:1.0:assertion", NULL, NULL },
        { "saml2", "urn:oasis:names:tc:SAML:2.0:assertion", NULL, NULL },
        { "wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd", NULL, NULL },
        { "xenc", "http://www.w3.org/2001/04/xmlenc#", NULL, NULL },
        { "wsc", "http://docs.oasis-open.org/ws-sx/ws-secureconversation/200512", "http://schemas.xmlsoap.org/ws/2005/02/sc", NULL },
        { "wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd", "http://docs.oasis-open.org/wss/oasis-wss-wssecurity-secext-1.1.xsd", NULL },
        { "dn", "http://www.onvif.org/ver10/network/wsdl", NULL, NULL },
        { "tns1", "http://www.onvif.org/ver10/topics", NULL, NULL },
        { NULL, NULL, NULL, NULL}
    };
	soap_set_namespaces(this->soap, namespaces);
}

void Service::destroy()
{	soap_destroy(this->soap);
	soap_end(this->soap);
}

void Service::reset()
{	this->destroy();
	soap_done(this->soap);
	soap_initialize(this->soap);
	Service_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

#ifndef WITH_PURE_VIRTUAL
Service *Service::copy()
{	Service *dup = SOAP_NEW_UNMANAGED(Service);
	if (dup)
	{	soap_done(dup->soap);
		soap_copy_context(dup->soap, this->soap);
	}
	return dup;
}
#endif

Service& Service::operator=(const Service& rhs)
{	if (this->soap != rhs.soap)
	{	if (this->soap_own)
			soap_free(this->soap);
		this->soap = rhs.soap;
		this->soap_own = false;
	}
	return *this;
}

int Service::soap_close_socket()
{	return soap_closesock(this->soap);
}

int Service::soap_force_close_socket()
{	return soap_force_closesock(this->soap);
}

int Service::soap_senderfault(const char *string, const char *detailXML)
{	return ::soap_sender_fault(this->soap, string, detailXML);
}

int Service::soap_senderfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_sender_fault_subcode(this->soap, subcodeQName, string, detailXML);
}

int Service::soap_receiverfault(const char *string, const char *detailXML)
{	return ::soap_receiver_fault(this->soap, string, detailXML);
}

int Service::soap_receiverfault(const char *subcodeQName, const char *string, const char *detailXML)
{	return ::soap_receiver_fault_subcode(this->soap, subcodeQName, string, detailXML);
}

void Service::soap_print_fault(FILE *fd)
{	::soap_print_fault(this->soap, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void Service::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this->soap, os);
}
#endif

char *Service::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this->soap, buf, len);
}
#endif

void Service::soap_noheader()
{	this->soap->header = NULL;
}

void Service::soap_header(char *wsa5__MessageID, struct wsa5__RelatesToType *wsa5__RelatesTo, struct wsa5__EndpointReferenceType *wsa5__From, struct wsa5__EndpointReferenceType *wsa5__ReplyTo, struct wsa5__EndpointReferenceType *wsa5__FaultTo, char *wsa5__To, char *wsa5__Action, struct chan__ChannelInstanceType *chan__ChannelInstance, wsa__AttributedURI *wsa__MessageID, wsa__Relationship *wsa__RelatesTo, wsa__EndpointReferenceType *wsa__From, wsa__EndpointReferenceType *wsa__ReplyTo, wsa__EndpointReferenceType *wsa__FaultTo, wsa__AttributedURI *wsa__To, wsa__AttributedURI *wsa__Action, wsd__AppSequenceType *wsd__AppSequence, int __size, char **__any, struct _wsse__Security *wsse__Security)
{
	::soap_header(this->soap);
	this->soap->header->wsa5__MessageID = wsa5__MessageID;
	this->soap->header->wsa5__RelatesTo = wsa5__RelatesTo;
	this->soap->header->wsa5__From = wsa5__From;
	this->soap->header->wsa5__ReplyTo = wsa5__ReplyTo;
	this->soap->header->wsa5__FaultTo = wsa5__FaultTo;
	this->soap->header->wsa5__To = wsa5__To;
	this->soap->header->wsa5__Action = wsa5__Action;
	this->soap->header->chan__ChannelInstance = chan__ChannelInstance;
	this->soap->header->wsa__MessageID = wsa__MessageID;
	this->soap->header->wsa__RelatesTo = wsa__RelatesTo;
	this->soap->header->wsa__From = wsa__From;
	this->soap->header->wsa__ReplyTo = wsa__ReplyTo;
	this->soap->header->wsa__FaultTo = wsa__FaultTo;
	this->soap->header->wsa__To = wsa__To;
	this->soap->header->wsa__Action = wsa__Action;
	this->soap->header->wsd__AppSequence = wsd__AppSequence;
	this->soap->header->__size = __size;
	this->soap->header->__any = __any;
	this->soap->header->wsse__Security = wsse__Security;
}

::SOAP_ENV__Header *Service::soap_header()
{	return this->soap->header;
}

#ifndef WITH_NOIO
int Service::run(int port, int backlog)
{	if (!soap_valid_socket(this->soap->master) && !soap_valid_socket(this->bind(NULL, port, backlog)))
		return this->soap->error;
	for (;;)
	{	if (!soap_valid_socket(this->accept()))
		{	if (this->soap->errnum == 0) // timeout?
				this->soap->error = SOAP_OK;
			break;
		}
		if (this->serve())
			break;
		this->destroy();
	}
	return this->soap->error;
}

#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
int Service::ssl_run(int port, int backlog)
{	if (!soap_valid_socket(this->soap->master) && !soap_valid_socket(this->bind(NULL, port, backlog)))
		return this->soap->error;
	for (;;)
	{	if (!soap_valid_socket(this->accept()))
		{	if (this->soap->errnum == 0) // timeout?
				this->soap->error = SOAP_OK;
			break;
		}
		if (this->ssl_accept() || this->serve())
			break;
		this->destroy();
	}
	return this->soap->error;
}
#endif

SOAP_SOCKET Service::bind(const char *host, int port, int backlog)
{	return soap_bind(this->soap, host, port, backlog);
}

SOAP_SOCKET Service::accept()
{	return soap_accept(this->soap);
}

#if defined(WITH_OPENSSL) || defined(WITH_GNUTLS)
int Service::ssl_accept()
{	return soap_ssl_accept(this->soap);
}
#endif
#endif

int Service::serve()
{
#ifndef WITH_FASTCGI
	this->soap->keep_alive = this->soap->max_keep_alive + 1;
#endif
	do
	{
#ifndef WITH_FASTCGI
		if (this->soap->keep_alive > 0 && this->soap->max_keep_alive > 0)
			this->soap->keep_alive--;
#endif
		if (soap_begin_serve(this->soap))
		{	if (this->soap->error >= SOAP_STOP)
				continue;
			return this->soap->error;
		}
		if ((dispatch() || (this->soap->fserveloop && this->soap->fserveloop(this->soap))) && this->soap->error && this->soap->error < SOAP_STOP)
		{
#ifdef WITH_FASTCGI
			soap_send_fault(this->soap);
#else
			return soap_send_fault(this->soap);
#endif
		}
#ifdef WITH_FASTCGI
		soap_destroy(this->soap);
		soap_end(this->soap);
	} while (1);
#else
	} while (this->soap->keep_alive);
#endif
	return SOAP_OK;
}

static int serve___wsd__Probe(struct soap*, Service*);
static int serve___wsd__ProbeMatches(struct soap*, Service*);

int Service::dispatch()
{	return dispatch(this->soap);
}

int Service::dispatch(struct soap* soap)
{
	Service_init(soap->imode, soap->omode);
	soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "wsd:Probe"))
		return serve___wsd__Probe(soap, this);
	if (!soap_match_tag(soap, soap->tag, "wsd:ProbeMatches"))
		return serve___wsd__ProbeMatches(soap, this);
	return soap->error = SOAP_NO_METHOD;
}

static int serve___wsd__Probe(struct soap *soap, Service *service)
{	struct __wsd__Probe soap_tmp___wsd__Probe;
	soap_default___wsd__Probe(soap, &soap_tmp___wsd__Probe);
	if (!soap_get___wsd__Probe(soap, &soap_tmp___wsd__Probe, "-wsd:Probe", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->Probe(soap_tmp___wsd__Probe.wsd__Probe);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}

static int serve___wsd__ProbeMatches(struct soap *soap, Service *service)
{	struct __wsd__ProbeMatches soap_tmp___wsd__ProbeMatches;
	soap_default___wsd__ProbeMatches(soap, &soap_tmp___wsd__ProbeMatches);
	if (!soap_get___wsd__ProbeMatches(soap, &soap_tmp___wsd__ProbeMatches, "-wsd:ProbeMatches", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = service->ProbeMatches(soap_tmp___wsd__ProbeMatches.wsd__ProbeMatches);
	if (soap->error)
		return soap->error;
	return soap_closesock(soap);
}
/* End of server object code */
