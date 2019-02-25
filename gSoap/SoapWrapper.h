#ifndef ONVIF_SOAPWRAPPER_H
#define ONVIF_SOAPWRAPPER_H

#include "soapH.h"
#include <gSoap/wsseapi.h>

#include "Authorization.h"

extern SOAP_NMAC struct Namespace soap_namespaces[];
extern SOAP_NMAC struct Namespace soap_vendors_namespaces[];

class SoapWrapper 
{
public:
	// Constructor allocates soap engine context, and sets namespace mapping table
	SoapWrapper(const std::string& endpoint, Onvif::AuthorizationHolder* authHolder, bool omodeCanonical = true, bool useWsAdressing = false);
	// Destructor frees deserialized data and soap engine context
	virtual ~SoapWrapper();

public:
	struct soap* getSoapStruct() const
	{
		return m_soapContext;
	}

	Onvif::AuthorizationHolder* getAuthHolder() const
	{
		return m_authHolder;
	}

	/// Binds socket to specified interface address and enables multicast option.
	/// Pass empty string to reset to default behavior.
	void enableMulticast(const std::string& interfaceAddress);
	SOAP_SOCKET wsMulticastOpen(struct soap* soap, const char* endpoint, const char* host, int port);
	const std::string& getEndpoint() const { return m_endpoint; }

private:
	// Runtime engine context allocated in constructor
	struct soap* const				m_soapContext;
	const std::string				m_endpoint;
	struct http_da_info				m_digestInfo;
	std::vector<std::string>		m_referenceParameters;

	// Device discovery stuff.
	typedef SOAP_SOCKET (*fopen_t)(struct soap*, const char*, const char*, int);
	fopen_t		m_defaultFopen;
	std::string	m_interfaceAddress;
    std::string m_header_action;

	Onvif::AuthState						m_authScheme;
	Onvif::AuthorizationHolder* const		m_authHolder;
	bool									m_useWsAdressing;



};


#endif // ONVIF_SOAPWRAPPER_H
