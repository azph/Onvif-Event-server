#ifndef ONVIF_SOAP_HELPERS_H
#define ONVIF_SOAP_HELPERS_H

#include <ctime>
#include <sstream>

#include <gSoap/soapH.h>

namespace SoapHelpers
{

std::chrono::milliseconds::rep getCurrentTime();

tt__DateTime* toDateTime(struct soap* soap, const std::tm* time);

timeval* convertTime(soap* soap, std::chrono::milliseconds::rep timeMicroseconds);

std::string getHost(struct soap* soap, const std::string& sufix = "");

std::string getUuidFromHost(const std::string& path, const std::string& sufix);

bool* soap_new_req_bool(struct soap* soap, bool value);

}

#endif

