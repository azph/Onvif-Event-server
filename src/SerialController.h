#ifndef SERIAL_CONTROLLER
#define SERIAL_CONTROLLER

#include <string>
#include <vector>
#include <algorithm>

#include <termios.h>

class SerialController
{
	SerialController() = delete;
	SerialController(const SerialController&) = delete;
	SerialController& operator=(SerialController) = delete;

	SerialController(const std::string& deviceFile, const int baudRate);
	~SerialController();

	public:
	static SerialController& GetInstance(const std::string& deviceFile, const int baudRate)
	{
		static SerialController instance {deviceFile, baudRate};
		return instance;
	}

	std::vector<uint8_t> ReadMessage();

	private:
	void connect();
	speed_t getRate(int rate);
	int serialDataAvail(const int fileDescriptor);

	private:
	std::string m_deviceFile;
	int m_baudRate;
	bool m_connected;
	int m_fileDescriptor;
};

#endif