#include "LicenseChecker.h"

#include <iostream>

#include <primitives/Logger.h>

#include <gSoap/thread_setup.h>

#include"OnvifServer.h"

std::promise<int> promise;

int main(int argc, char *argv[])
{
	LOG_INFO << "----------------------------------------------------------------------";
#ifndef WIN32
	if (!isKeyValid())
	{
		LOG_ERROR << "License key is invalid!";
		return -1;
	}
#endif
	
	// make OpenSSL MT-safe with mutex
	CRYPTO_thread_setup();

	Onvif::OnvifServer::getInstance().start();


	std::future<int> future = promise.get_future();
	std::atexit([]()
	{
		LOG_INFO << "atexit begin";
		Onvif::OnvifServer::getInstance().stop();
		// clean up OpenSSL mutex
		CRYPTO_thread_cleanup();
		LOG_INFO << "atexit end";
		promise.set_value(0);
	});

	int value = future.get();
	LOG_INFO << "Exit ";
	return 0;
}
