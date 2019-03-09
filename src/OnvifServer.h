#ifndef ONVIF_SERVER_H
#define ONVIF_SERVER_H

#include <thread>
#include <future>

namespace Onvif
{

class OnvifServer 
{
public:

	static OnvifServer& getInstance();

	void start();
	void stop();
	
private:
	OnvifServer();
	~OnvifServer();
		
	OnvifServer(const OnvifServer&) = delete;
	OnvifServer& operator= (const OnvifServer&) = delete;

	void onStartServices();
	
private:
	const std::uint32_t m_maxThreadCount;
	std::thread m_serverTread;

};
}

#endif

