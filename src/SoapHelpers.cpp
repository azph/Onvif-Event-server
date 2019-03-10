#include "SoapHelpers.h"


namespace SoapHelpers
{

std::uint64_t getCurrentTime()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

tt__DateTime* toDateTime(struct soap* soap, const std::tm* time)
{
	tt__DateTime* result = soap_new_tt__DateTime(soap);

	// Time.
	result->Time = soap_new_tt__Time(soap);
	result->Time->Hour = time->tm_hour;
	result->Time->Minute = time->tm_min;
	result->Time->Second = time->tm_sec;

	// Date.
	result->Date = soap_new_tt__Date(soap);
	result->Date->Year = time->tm_year + 1900; // Start of epoch.
	result->Date->Month = time->tm_mon + 1;
	result->Date->Day = time->tm_mday;

	return result;
}

timeval* convertTime(soap* soap, std::time_t timeMicroseconds)
{
	timeval* tm = soap_new_xsd__dateTime(soap);

	tm->tv_sec = static_cast<long>(timeMicroseconds / 1000000);
	tm->tv_usec = static_cast<long>(timeMicroseconds % 1000000);

	return tm;
}

std::string getHost(struct soap* soap, char* sufix)
{
	std::ostringstream host;
	host << "http://" << soap->host << ":" << soap->proxy_port << soap->path << sufix;
	return host.str();
}

bool* soap_new_req_bool(struct soap* soap, bool value)
{
	auto result = soap_new_bool(soap);
	*result = value;

	return result;
}

}