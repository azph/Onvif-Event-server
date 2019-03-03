#include <string>
#include <regex>
#include <fstream>

#include "sysinfo.h"

std::string GetMac()
{
	//MAC is unique for each network adapter
	
	std::ifstream inFile("/sys/class/net/eth0/address");
	if(!inFile.is_open())
		throw std::runtime_error("Can't open eth0 file");
		
	std::string mac;
	inFile >> mac;
	inFile.close();
	
	return mac;
}

std::string GetMicroSDCID()
{
	// CID - Card Identification Data
	
	std::ifstream inFile("/sys/block/mmcblk0/device/cid");
	if(!inFile.is_open())
		throw std::runtime_error("Can't open cid file");
	
	std::string cid;
	inFile >> cid;
	inFile.close();
	
	return cid;
}

std::string GetCPUSerial()
{
	std::ifstream inFile("/proc/cpuinfo");
	if(!inFile.is_open())
		throw std::runtime_error("Can't open cpuinfo file");
	
	std::stringstream stream;
	stream << inFile.rdbuf();
	inFile.close();
	
	const auto cpuinfo = stream.str();
	
	// Serial number is unique value for each ARM CPU
	const std::regex reg("Serial\\s*:\\s*(\\w*\\S)");
	std::smatch match;
	std::regex_search(cpuinfo, match, reg);
	
	if (match.size() < 2)
		throw std::runtime_error("Can't parse Serial from cpuinfo");
	
	// match[0] Serial		: 0000000060036360
	// match[1] 0000000060036360
	
	std::string cpuid = match[1];
	
	return cpuid;
}

