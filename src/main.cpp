#include "LicenseChecker.h"

#include <iostream>

#include <gSoap/thread_setup.h>

#include"OnvifServer.h"


int main(int argc, char *argv[])
{
#ifndef WIN32
	if (!isKeyValid())
	{
		std::cerr << "Licene key is invalid" << std::endl;
		return -1;
	}
#endif
	
	// make OpenSSL MT-safe with mutex
	CRYPTO_thread_setup();

	Onvif::OnvifServer::getInstance().start();


	Onvif::OnvifServer::getInstance().stop();
	// clean up OpenSSL mutex
	CRYPTO_thread_cleanup();

	return 0;
}
