#include "SoapHelpers.h"

namespace SoapHelpers
{

std::chrono::milliseconds::rep getCurrentTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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

timeval* convertTime(soap* soap, std::chrono::milliseconds::rep timeMiliseconds)
{
	timeval* tm = soap_new_xsd__dateTime(soap);

	tm->tv_sec = static_cast<long>(timeMiliseconds / 1000);
	tm->tv_usec = static_cast<long>(timeMiliseconds % 1000) * 1000;

	return tm;
}

std::string getHost(struct soap* soap, const std::string& sufix)
{
	std::string endpoint = soap->endpoint;
	std::ostringstream port;
	port << ":" << soap->proxy_port;
	if (endpoint.find(port.str() + soap->path) != std::string::npos)
	{
		return endpoint + sufix;
	}
	auto pos = endpoint.find(soap->path);

	if (pos != std::string::npos)
	{
		std::ostringstream host;
		host << endpoint.substr(0, pos) << port.str() << soap->path << sufix;
		return host.str();
	}
	
	return endpoint;
}

std::string getUuidFromHost(const std::string& path, const std::string& sufix)
{
	auto pos = path.find(sufix);
	if (pos != std::string::npos)
	{
		return path.substr(pos + sufix.size(), path.size());
	}

	return {};
}

bool* soap_new_req_bool(struct soap* soap, bool value)
{
	auto result = soap_new_bool(soap);
	*result = value;

	return result;
}


}