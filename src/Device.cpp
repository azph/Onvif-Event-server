#include "Device.h"

#include <set>
#include <map>
#include <ctime>
#include <sstream>

#include "AuthorisationHolder.h"

namespace Onvif
{

Device::Device(struct soap *_soap):
	DeviceBindingService(_soap)
{
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

std::string getHost(struct soap* soap, char* sufix = "")
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
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
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
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	
	auto host = getHost(soap);
	// Device service.
	auto deviceService = soap_new_tds__Service(soap);
	deviceService->Namespace = SOAP_NAMESPACE_OF_tds;
	deviceService->XAddr = host;
	deviceService->Version = soap_new_req_tt__OnvifVersion(soap, 2, 4);
	if (tds__GetServices->IncludeCapability)
	{
		deviceService->Capabilities = soap_new_req__tds__Service_Capabilities(soap);
		deviceService->Capabilities->__any.set(createCapabilities(), SOAP_TYPE_tds__DeviceServiceCapabilities);
	}
	tds__GetServicesResponse.Service.push_back(deviceService);

	// Event service.
	auto eventService = soap_new_tds__Service(soap);
	eventService->Namespace = SOAP_NAMESPACE_OF_tev;
	eventService->XAddr = host;
	eventService->Version = soap_new_req_tt__OnvifVersion(soap, 2, 4);
	tds__GetServicesResponse.Service.push_back(eventService);

	return SOAP_OK;
}

tds__DeviceServiceCapabilities* Device::createCapabilities()
{
	auto deviceCapabilities = soap_new_req_tds__DeviceServiceCapabilities(soap,
		soap_new_req_tds__NetworkCapabilities(soap),
		soap_new_req_tds__SecurityCapabilities(soap),
		soap_new_req_tds__SystemCapabilities(soap));

	deviceCapabilities->Security->HttpDigest = soap_new_req_bool(soap, true);
	deviceCapabilities->Security->UsernameToken = soap_new_req_bool(soap, true);

	deviceCapabilities->System->DiscoveryBye = soap_new_req_bool(soap, true);
	deviceCapabilities->System->DiscoveryResolve = soap_new_req_bool(soap, true);
	deviceCapabilities->System->RemoteDiscovery = soap_new_req_bool(soap, false);
	deviceCapabilities->System->SystemBackup = soap_new_req_bool(soap, false);
	deviceCapabilities->System->SystemLogging = soap_new_req_bool(soap, false);
	deviceCapabilities->System->FirmwareUpgrade = soap_new_req_bool(soap, false);

	return deviceCapabilities;
}

int Device::GetServiceCapabilities(_tds__GetServiceCapabilities *tds__GetServiceCapabilities, _tds__GetServiceCapabilitiesResponse &tds__GetServiceCapabilitiesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	tds__GetServiceCapabilitiesResponse.Capabilities = createCapabilities();
	return SOAP_OK;
}

int Device::GetScopes(_tds__GetScopes *tds__GetScopes, _tds__GetScopesResponse &tds__GetScopesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
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
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	tds__GetCapabilitiesResponse.Capabilities = soap_new_tt__Capabilities(soap);
	
	// Device.
	auto deviceCapabilities = soap_new_tt__DeviceCapabilities(soap);

	deviceCapabilities->XAddr = getHost(soap);
	deviceCapabilities->System = soap_new_tt__SystemCapabilities(soap);
	deviceCapabilities->System->DiscoveryResolve = true;
	deviceCapabilities->System->DiscoveryBye = true;
	deviceCapabilities->System->RemoteDiscovery = false;
	deviceCapabilities->System->SystemBackup = false;
	deviceCapabilities->System->SystemLogging = false;
	deviceCapabilities->System->FirmwareUpgrade = false;

	auto version = soap_new_tt__OnvifVersion(soap);
	version->Major = 2;
	version->Minor = 4;
	deviceCapabilities->System->SupportedVersions.push_back(version);

	tds__GetCapabilitiesResponse.Capabilities->Device = deviceCapabilities;

	// Event.
	tds__GetCapabilitiesResponse.Capabilities->Events = soap_new_req_tt__EventCapabilities(soap,
		getHost(soap), false, false, false);

	return SOAP_OK;
}

int Device::GetNetworkInterfaces(_tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, _tds__GetNetworkInterfacesResponse &tds__GetNetworkInterfacesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
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
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
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