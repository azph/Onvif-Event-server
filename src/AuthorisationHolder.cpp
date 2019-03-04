#include "AuthorisationHolder.h"

#include <gSoap/httpda.h>

namespace Onvif
{

	Onvif::AuthorisationHolder& AuthorisationHolder::getInstance()
	{
		static AuthorisationHolder holder;

		return holder;
	}

bool AuthorisationHolder::verifyPassword(struct soap *soap)
{
	static const std::string authrealm = "WEB SERVER";
	static const std::string passwd = "0eydozFnrrsF";
	static const std::string userid = "admin";

	if (soap->authrealm && soap->userid && authrealm == soap->authrealm && userid == soap->userid
		&& http_da_verify_post(soap, passwd.c_str()) == SOAP_OK)
	{
		soap_wsse_delete_Security(soap);
		return true;
	}

	if (soap->header && soap->header->wsse__Security)
	{
		const char *username = soap_wsse_get_Username(soap);

		if (username && username == userid && soap_wsse_verify_Password(soap, passwd.c_str()) == SOAP_OK)
		{
			soap_wsse_delete_Security(soap);
			return true;
		}
	}

	soap->authrealm = authrealm.c_str();
	soap_wsse_delete_Security(soap);
	return false;
}

}