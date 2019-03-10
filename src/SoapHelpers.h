#ifndef ONVIF_SOAP_HELPERS_H
#define ONVIF_SOAP_HELPERS_H

#include <ctime>
#include <sstream>

#include <gSoap/soapH.h>

namespace SoapHelpers
{

std::uint64_t getCurrentTime();

tt__DateTime* toDateTime(struct soap* soap, const std::tm* time);

timeval* convertTime(soap* soap, std::time_t timeMicroseconds);

std::string getHost(struct soap* soap, char* sufix = "");

bool* soap_new_req_bool(struct soap* soap, bool value);

}

#endif

