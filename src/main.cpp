#include "LicenseChecker.h"

#include <iostream>

#include <primitives/Logger.h>

#include <gSoap/thread_setup.h>

#include"OnvifServer.h"

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


	Onvif::OnvifServer::getInstance().stop();
	// clean up OpenSSL mutex
	CRYPTO_thread_cleanup();

	LOG_INFO << "Exit ";
	return 0;
}
