#include "Device.h"

#include <set>
#include <map>
#include <ctime>

#include <gSoap/httpda.h>

namespace Onvif
{

Device::Device(struct soap *_soap):
	DeviceBindingService(_soap)
{
}

bool Device::verifyPasswordDA()
{
	static const std::string authrealm = "WEB SERVER";
	static const std::string passwd = "0eydozFnrrsF";
	static const std::string userid = "admin";

	if (!soap->authrealm || !soap->userid || authrealm != soap->authrealm || userid != soap->userid
		|| http_da_verify_post(soap, passwd.c_str()) != SOAP_OK)
	{
		soap->authrealm = authrealm.c_str();
		return false;
	}
	return true;
}

tt__DateTime* toDateTime(struct soap* soap, std::tm* time)
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

int Device::GetSystemDateAndTime(_tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, _tds__GetSystemDateAndTimeResponse &tds__GetSystemDateAndTimeResponse)
{
	// local
	std::time_t lt = std::time(0);
	std::tm* now = std::localtime(&lt);

	auto systemDateTime = soap_new_tt__SystemDateTime(soap);

	systemDateTime->DateTimeType = tt__SetDateTimeType::Manual;
	systemDateTime->DaylightSavings = now->tm_isdst;
	
	systemDateTime->TimeZone = soap_new_tt__TimeZone(soap);
	//systemDateTime->TimeZone->TZ = *_tzname;

	systemDateTime->LocalDateTime = toDateTime(soap, now);
	
	// utc
	std::time_t utc = std::time(0);
	std::tm* gmt = std::gmtime(&utc);
	systemDateTime->UTCDateTime = toDateTime(soap, gmt);

	tds__GetSystemDateAndTimeResponse.SystemDateAndTime = systemDateTime;

	return SOAP_OK;
}

int Device::GetDeviceInformation(_tds__GetDeviceInformation *tds__GetDeviceInformation, _tds__GetDeviceInformationResponse &tds__GetDeviceInformationResponse)
{
	if (!verifyPasswordDA())
	{
		return 401;
	}

	tds__GetDeviceInformationResponse.Manufacturer = "Sphinx";
	tds__GetDeviceInformationResponse.Model = "OKO-STRIM";
	tds__GetDeviceInformationResponse.FirmwareVersion = "1.0.0";
	tds__GetDeviceInformationResponse.SerialNumber = "1.0.0";
	tds__GetDeviceInformationResponse.HardwareId = "1.0.0";
	return SOAP_OK;
}

int Device::GetServices(_tds__GetServices *tds__GetServices, _tds__GetServicesResponse &tds__GetServicesResponse)
{
	if (!verifyPasswordDA())
	{
		return 401;
	}

	// Device service.
	auto deviceService = soap_new_tds__Service(soap);
	deviceService->Namespace = SOAP_NAMESPACE_OF_tds;
	deviceService->XAddr = soap->endpoint;
	tds__GetServicesResponse.Service.push_back(deviceService);

	// Event service.
	auto eventService = soap_new_tds__Service(soap);
	eventService->Namespace = SOAP_NAMESPACE_OF_tds;
	eventService->XAddr = soap->endpoint;
	tds__GetServicesResponse.Service.push_back(eventService);

	return SOAP_OK;
}

int Device::GetScopes(_tds__GetScopes *tds__GetScopes, _tds__GetScopesResponse &tds__GetScopesResponse)
{
	if (!verifyPasswordDA())
	{
		return 401;
	}

	const std::map<std::string, tt__ScopeDefinition> scopes =
	{
		{ "onvif://www.onvif.org/Profile/C", tt__ScopeDefinition::Fixed },
		{ "onvif://www.onvif.org/type/", tt__ScopeDefinition::Fixed },
		{ "onvif://www.onvif.org/hardware/OKO-STRIM", tt__ScopeDefinition::Fixed },
		{ "onvif://www.onvif.org/name/Sphinx", tt__ScopeDefinition::Fixed },
		{ "onvif://www.onvif.org/location/country/russia", tt__ScopeDefinition::Fixed }
	};

	for (const auto& value : scopes)
	{
		auto scope = soap_new_tt__Scope(soap);
		scope->ScopeItem = value.first;
		scope->ScopeDef = value.second;

		tds__GetScopesResponse.Scopes.push_back(scope);
	}

	return SOAP_OK;
}

int Device::GetCapabilities(_tds__GetCapabilities *tds__GetCapabilities, _tds__GetCapabilitiesResponse &tds__GetCapabilitiesResponse)
{
	return SOAP_OK;
}

int Device::GetNetworkInterfaces(_tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, _tds__GetNetworkInterfacesResponse &tds__GetNetworkInterfacesResponse)
{
	if (!verifyPasswordDA())
	{
		return 401;
	}

	auto networkInterface = soap_new_tt__NetworkInterface(soap);

	networkInterface->Enabled = true;
	networkInterface->token = "eth0";

	networkInterface->Info = soap_new_tt__NetworkInterfaceInfo(soap);
	// TODO: MAC Address.
	networkInterface->Info->HwAddress = ""; 
	networkInterface->Info->Name = soap_new_std__string(soap);
	*networkInterface->Info->Name = networkInterface->token;

	auto ipv4 = soap_new_tt__IPv4NetworkInterface(soap);
	ipv4->Enabled = true;
	ipv4->Config = soap_new_tt__IPv4Configuration(soap);
	ipv4->Config->DHCP = false;
	auto prefixed = soap_new_tt__PrefixedIPv4Address(soap);
	prefixed->Address = soap->host;
	prefixed->PrefixLength = 16;
	ipv4->Config->Manual.push_back(prefixed);
	networkInterface->IPv4 = ipv4;

	tds__GetNetworkInterfacesResponse.NetworkInterfaces.push_back(networkInterface);

	return SOAP_OK;
}

int Device::GetZeroConfiguration(_tds__GetZeroConfiguration *tds__GetZeroConfiguration, _tds__GetZeroConfigurationResponse &tds__GetZeroConfigurationResponse)
{
	if (!verifyPasswordDA())
	{
		return 401;
	}

	tds__GetZeroConfigurationResponse.ZeroConfiguration = soap_new_tt__NetworkZeroConfiguration(soap);
	tds__GetZeroConfigurationResponse.ZeroConfiguration->Addresses.push_back("");
	tds__GetZeroConfigurationResponse.ZeroConfiguration->Enabled = false;
	tds__GetZeroConfigurationResponse.ZeroConfiguration->InterfaceToken = "eth0";
	return SOAP_OK;
}

}