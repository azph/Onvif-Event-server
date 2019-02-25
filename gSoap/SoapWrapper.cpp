#include "SoapWrapper.h"

#include <gSoap/wsaapi.h>

#include "soap.nsmap"


SoapWrapper::SoapWrapper(const std::string& endpoint, Onvif::AuthorizationHolder* authHolder, bool omodeCanonical, bool useWsAdressing) :
	m_soapContext(soap_new()),
	m_endpoint(endpoint),
	m_digestInfo(),
	m_defaultFopen(0),
	m_authScheme(Onvif::AUTH_NOT_USED),
	m_authHolder(authHolder),
	m_useWsAdressing(useWsAdressing)
{

	if (!m_soapContext)
	{
		throw std::runtime_error("Failed to create gSOAP runtime engine context");
	}
	m_soapContext->namespaces = soap_namespaces;
	if (omodeCanonical)
	{
		soap_omode(m_soapContext, SOAP_XML_CANONICAL);
	}
	m_soapContext->bind_flags = SO_REUSEADDR;

	static const int TIME_OUT_IN_SECONDS = 10;
	m_soapContext->connect_timeout = TIME_OUT_IN_SECONDS;
	m_soapContext->send_timeout = TIME_OUT_IN_SECONDS;
	m_soapContext->recv_timeout = TIME_OUT_IN_SECONDS;
	m_soapContext->accept_timeout = TIME_OUT_IN_SECONDS;


	if (soap_register_plugin(m_soapContext, &http_da) != SOAP_OK)
	{
		throw std::runtime_error("Can't register HTTP digest plugin for gSoap framework!");
	}

	if (m_useWsAdressing && soap_register_plugin(getSoapStruct(), soap_wsa) != SOAP_OK)
	{
		throw std::runtime_error("Can't register WS-Addressing plugin for gSoap framework!");
	}

	// Workaround of bug in gSoap.
	// We have to make additional initialization of internal structure of gSoap.
	if (http_da_data* data = static_cast<http_da_data*>(soap_lookup_plugin(m_soapContext, http_da_id)))
	{
		data->nonce = 0;
		data->opaque = 0;
		data->qop = 0;
		data->alg = 0;
		data->nc = 0;
		data->ncount = 0;
		data->cnonce = 0;
		memset(reinterpret_cast<void*>(data->response), 0, sizeof(data->response));
	}

	if (m_authHolder)
	{
		const Onvif::AuthorizationInfo info = m_authHolder->getAuthorization();
		m_authScheme = info.m_authScheme;
		m_digestInfo = info.m_digestInfo.duplicate();
	}

	//m_soapContext->user = this;
}

SoapWrapper::~SoapWrapper()
{
	http_da_release(m_soapContext, &m_digestInfo); // remove userid and passwd 
	soap_destroy(m_soapContext);
	soap_end(m_soapContext);
	soap_free(m_soapContext);
}


void SoapWrapper::enableMulticast(const std::string& interfaceAddress)
{
#ifdef _MSC_VER
	m_interfaceAddress = interfaceAddress;

	// Reset to default.
	if (interfaceAddress.empty())
	{
		if (m_defaultFopen)
		{
			m_soapContext->fopen = m_defaultFopen;
			m_defaultFopen = 0;
		}
		return;
	}

	/// Open function may be already overridden.
	if (!m_defaultFopen)
	{
		m_defaultFopen = m_soapContext->fopen;
		//m_soapContext->fopen = &wsMulticastOpen;
	}
#endif
}

#ifdef _MSC_VER
SOAP_SOCKET SoapWrapper::wsMulticastOpen(struct soap* soap, const char* endpoint, const char* host, int port)
{
	if (!soap->user)
		return INVALID_SOCKET;

	SoapWrapper* const wrapper = static_cast<SoapWrapper*>(soap->user);
	if (!wrapper || !wrapper->m_defaultFopen || !host)
		return INVALID_SOCKET;

	try
	{
		
		// Duplicate socket handle to prevent close socket from asio udp class destructor.
		WSAPROTOCOL_INFO protInfo = {0};
		//int erroCode = WSADuplicateSocket(udpSocket.native(), GetCurrentProcessId(), &protInfo);
		return WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &protInfo, 0, 0);
	}
	catch (const std::exception& e)
	{
	}
	return INVALID_SOCKET;
}

#endif


void soapLogMessage(const struct soap* soapStruct, int index, const char* msg, size_t size)
{
	if (!soapStruct)
		return;

	SoapWrapper* wrapper = static_cast<SoapWrapper*>(soapStruct->user);
	//if (wrapper)
		//wrapper->processLogging(index, msg, size);
}
