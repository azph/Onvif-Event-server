#ifndef REAL_TIME_CLOCK
#define REAL_TIME_CLOCK

#include <string>
#include <mutex>
#include <ctime>

class RealTimeClock
{
	RealTimeClock() = delete;
	RealTimeClock(const RealTimeClock&) = delete;
	RealTimeClock& operator= (RealTimeClock&) = delete;
	
	RealTimeClock(std::string& deviceFile, uint16_t slaveAddress);
	~RealTimeClock();
	
	public:
	static RealTimeClock& GetInstance(std::string& deviceFile, uint16_t slaveAddress)
	{
		static RealTimeClock instance{deviceFile, slaveAddress};
		return instance;
	}
	
	void Connect();
	void SetTime(std::tm& time);
	std::tm GetTime();
	
	private:
	uint8_t convertFromRegisterFormat(uint8_t input);
	uint8_t convertToRegisterFormat(uint8_t input);
	
	private:
	bool m_connected;
	int m_fileDescriptor;
	std::string m_deviceFile;
	uint16_t m_slaveAddress;
	std::mutex m_mutex;
};

#endif
