#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <thread>
////
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>
////
#include "SerialController.h"
#include "EventReader.h"
#include "SoapHelpers.h"

#define LOW_BYTE_EXTRACT(toByteDigit) toByteDigit & 0b0000000011111111

// Packet common fileds position
#define START_BYTE 0
#define MSB 1
#define LSB 2
#define FRAME_TYPE 3
#define SRC_ID_MSB 4
#define SRC_ID_LSB 5

#define START_BYTE_VALUE 0x7e

// Device types and their messages
#define FRAME_DETECTOR 0x08
#define FD_METAL_DETECTED 0x01
#define FD_RAD_DETECTED 0x05
#define FD_EXPLOSIVE_DETECTED 0x07

#define EXPLOSIVE_DETECTOR 0x07
#define ED_EXPLOSIVE_DETECTED 0x01

#define HANDHELD_DETECTOR_1 0x1
#define HANDHELD_DETECTOR_2 0x2
#define HANDHELD_DETECTOR_3 0x3
#define HANDHELD_DETECTOR_4 0x4

uint16_t combine2Bytes(uint8_t high, uint8_t low)
{
	uint16_t result = 0;
	result = high;
	// Set high bits left
	result <<= 8;
	// Set low bits
	return result | low;
}

bool checkHashSumm(const std::vector<uint8_t>& message)
{
	// Ignore first 3 byte
	const uint16_t CHECKABLE_DATA_POSITION = 3;
	uint16_t checkSumm = std::accumulate(message.begin() + CHECKABLE_DATA_POSITION, message.end(), 0);

	return (LOW_BYTE_EXTRACT(checkSumm)) == 0xff;
}

uint8_t calcHashSumm(const std::vector<uint8_t>& message)
{
	// Ignore first 3 byte and last byte
	const uint16_t CHECKABLE_DATA_POSITION = 3;
	uint16_t checkSumm = std::accumulate(message.begin() + CHECKABLE_DATA_POSITION, message.end() - 1, 0);
	//const uint16_t LOW_BYTE_EXTRACT = 0b0000000011111111;

	return 0xff - LOW_BYTE_EXTRACT(checkSumm);
}

std::string parseUartPacket(const std::vector<uint8_t>& mes, std::vector<NotificationMessage>& result)
{
	const uint8_t frameType = mes[FRAME_TYPE];
	uint8_t dataStartPos = 0;

	switch (frameType)
	{
	case 0x8f:
		dataStartPos = 8;
		break;
	case 0x83:
		dataStartPos = 10;
		break;
	default:
		// unknow frameType
		// log and exit
		break;
	}

	uint16_t sourceID = combine2Bytes(mes[SRC_ID_MSB], mes[SRC_ID_LSB]);

	uint8_t comandCode = mes[dataStartPos];

	// Ignore command id byte
	auto dataBuff = mes.begin() + dataStartPos + 1;
	// Ignore control summ byte (last byte in)

	std::stringstream resultMessge;

	if (sourceID == FRAME_DETECTOR)
	{
		switch (comandCode)
		{
		case FD_METAL_DETECTED:
		{
			resultMessge << "Metal detected";
			NotificationMessage messageMetall = { MessageType::MetallDetector, "+", std::chrono::milliseconds(SoapHelpers::getCurrentTime()) };
			result.push_back(messageMetall);
			return resultMessge.str();
		}
		case FD_EXPLOSIVE_DETECTED:
		{
			resultMessge << (uint16_t)dataBuff[0] << ", " << (uint16_t)dataBuff[1] << ", " << (uint16_t)dataBuff[2] << ", " << (uint16_t)dataBuff[3];
			NotificationMessage messageExplosive = { MessageType::SteamDetector, resultMessge.str(), std::chrono::milliseconds(SoapHelpers::getCurrentTime())};
			result.push_back(messageExplosive);
			return resultMessge.str();
		}
		case FD_RAD_DETECTED:
		{
			resultMessge << "More than 0.3mSv / h";
			NotificationMessage messageRad = { MessageType::RadiationMonitoring, resultMessge.str(), std::chrono::milliseconds(SoapHelpers::getCurrentTime())};
			result.push_back(messageRad);
			return resultMessge.str();
		}
		default:
			return "";
		}
	}
	if (sourceID == EXPLOSIVE_DETECTOR)
	{
		switch (comandCode)
		{
		case ED_EXPLOSIVE_DETECTED:
		{
			std::cout << "Im here" << std::endl;
			resultMessge << (uint16_t)dataBuff[0] << ", " << (uint16_t)dataBuff[1] << ", " << (uint16_t)dataBuff[2] << ", " << (uint16_t)dataBuff[3] << ", ";
			resultMessge << (uint16_t)dataBuff[4] << ", " << (uint16_t)dataBuff[5] << ", " << (uint16_t)dataBuff[6] << ", " << (uint16_t)dataBuff[7] << ", ";
			resultMessge << (uint16_t)dataBuff[8] << ", " << (uint16_t)dataBuff[9] << ", " << (uint16_t)dataBuff[10] << ", " << (uint16_t)dataBuff[11] << ", ";
			resultMessge << (uint16_t)dataBuff[12] << ", " << (uint16_t)dataBuff[13] << ", " << (uint16_t)dataBuff[14] << ", " << (uint16_t)dataBuff[15] << ", ";
			resultMessge << (uint16_t)dataBuff[16] << ", " << (uint16_t)dataBuff[17] << ", " << (uint16_t)dataBuff[18] << ", " << (uint16_t)dataBuff[19];
			NotificationMessage messageExplosive = { MessageType::SteamDetector, resultMessge.str(), std::chrono::milliseconds(SoapHelpers::getCurrentTime())};
			result.push_back(messageExplosive);
			return resultMessge.str();
			break;
		}
		default:
			// unknow message
			break;
		}
	}
	else if (sourceID == HANDHELD_DETECTOR_1 || sourceID == HANDHELD_DETECTOR_2
		|| sourceID == HANDHELD_DETECTOR_3 || sourceID == HANDHELD_DETECTOR_4)
	{


	}
	else
	{
		// unknow source id
		// return, log
	}

	
}



EventReader::EventReader()
{
	SerialController::GetInstance();
}

EventReader::~EventReader()
{

}


std::vector<NotificationMessage> EventReader::ReadEvents()
{
	std::vector<NotificationMessage> result;
	auto& controller = SerialController::GetInstance();

	auto data = controller.ReadMessage();

	if(data.empty())
	{
		return result;
	}

	std::stringstream stream;
	for(auto& value : data)
		stream << std::hex << (uint16_t)value;
	std::cerr << stream.str() << std::endl;

	m_buffer.insert(m_buffer.end(), data.begin(), data.end());

	while (!m_buffer.empty())
	{
		auto it = std::find(m_buffer.begin(), m_buffer.end(), START_BYTE_VALUE);

		m_buffer.erase(m_buffer.begin(), it);	

		if (m_buffer.size() <= 4)
		{
			return result;
		}
		auto& msbValue = *(++it);
		auto& lsbValue = *(++it);

		const uint16_t frameLength = combine2Bytes(msbValue, lsbValue);

		if (m_buffer.size() < frameLength + 3)
		{
			return result;
		}

		std::vector<uint8_t> mes;
		auto end = m_buffer.begin();
		std::advance(end, frameLength + 3);
		std::copy(m_buffer.begin(), end, std::back_inserter(mes));
		m_buffer.erase(m_buffer.begin(), end);
		// check summ
		auto event = parseUartPacket(mes, result);
		
	}
}
