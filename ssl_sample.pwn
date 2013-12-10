#include <socket>

// sample function of how to create a SSL server
stock TCP_SSL_server()
{
	new Socket:sock = socket_create(TCP);
	if(is_socket_valid(sock)) {
	    ssl_init();
	    ssl_create_context(sock, METHOD_SERVER);
	    ssl_load_cert_into_context(sock, "path/to/certifate.crt", "path/to/key.key");
	    socket_set_max_connections(sock, 10);
	    ssl_set_accept_timeout(sock, 3000);
	    socket_listen(sock, 1337);
	    return 1;
	}
	return 0;
}

// sample function for a SSL client
stock TCP_SSL_client()
{
	new Socket:sock = socket_create(TCP);
	if(is_socket_valid(sock)) {
	    ssl_init();
	    ssl_create_context(sock, METHOD_CLIENT);
	    if(socket_connect(sock, "somesite.com", 443)) {
	        if(ssl_connect(sock)) {
	            // our ssl context (object) has successfully connected to our socket handle
	            //socket_send(sock, "GET /\r\n\r\n", 13);
	            return 1;
			}
		}
	}
	return 0;
}
