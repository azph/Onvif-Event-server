#include "Device.h"
#include "Event.h"

#include <gSoap/thread_setup.h>
#include <gSoap/httpda.h>
#include <gSoap/wsaapi.h>

int main(int argc, char *argv[])
{
	// make OpenSSL MT-safe with mutex
	CRYPTO_thread_setup();

	struct soap *soap = soap_new();
	soap_register_plugin_arg(soap, http_da, http_da_md5());
	soap_register_plugin(soap, soap_wsa);

	auto device = std::make_shared<Onvif::Device>(soap);
	auto event = std::make_shared<Onvif::Event>(soap);

	if (!soap_valid_socket(soap_bind(soap, NULL, 8080, 100)))
		exit(EXIT_FAILURE);
	soap->max_keep_alive = 0;
	while (1)
	{
		if (!soap_valid_socket(soap_accept(soap)))
			exit(EXIT_FAILURE);
		int err = 0;

		if (soap_begin_serve(soap))
		{
			soap_stream_fault(soap, std::cerr);
		}
		else if (err = device->dispatch() == SOAP_NO_METHOD)
		{
			err = event->dispatch();
		}

		if (err)
		{
			soap_send_fault(soap);
		}

		soap_destroy(soap);
		soap_end(soap);
	}

	soap_free(soap); // safe to delete when abc, uvw, xyz are also deleted

	// clean up OpenSSL mutex
	CRYPTO_thread_cleanup();

	return 0;
}
