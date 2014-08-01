/**
 *
 * Socket Plugin v0.2b
 * (c) BlueG 2012-2014
 *
 */
#if defined socket_included
	#endinput
#endif
#define socket_included

#define INVALID_SOCKET (-1)
#define INVALID_CLIENT_ID (-1)
#define NO_IP_RETURN "0.0.0.0"
#define INADDR_ANY NO_IP_RETURN

// SSL related
#define METHOD_CLIENT 0 //v23
#define METHOD_SERVER 1 //v23

enum pType {
	TCP = 1,
	UDP = 2
};

// ssl.h
enum SSL_mode {
	SSL_MODE_ENABLE_PARTIAL_WRITE = 0x00000001,
	SSL_MODE_ACCEPT_MOVING_BUFFER = 0x00000002,
	SSL_MODE_AUTO_RETRY = 0x00000004,
	SSL_MODE_RELEASE_BUFFERS = 0x00000010
};

native Socket:socket_create(pType:TCP); // udp & tcp
native socket_bind(Socket:id, ip[]); // udp & tcp
native socket_connect(Socket:id, host[], port); // udp & tcp
native socket_listen(Socket:id, port); // udp & tcp
native socket_stop_listen(Socket:id); // udp & tcp
native socket_destroy(Socket:id); // udp & tcp
native socket_send(Socket:id, data[], len); // udp & tcp
native socket_sendto(Socket:id, const ip[], port, data[], len);
native socket_send_array(Socket:id, data[], size=sizeof(data));
native is_socket_valid(Socket:id); // udp & tcp
native socket_set_max_connections(Socket:id, max_remote_clients); // tcp only
native socket_close_remote_client(Socket:id, remote_clientid); // tcp only
native socket_sendto_remote_client(Socket:id, remote_clientid, data[]); // tcp only
native socket_remote_client_connected(Socket:id, remote_clientid); // tcp only
native get_remote_client_ip(Socket:id, remote_clientid, ip[]); // tcp only

// ssl stuff

native ssl_init(); // initialize the ssl library
native ssl_create_context(Socket:id, method);
native ssl_connect(Socket:id); // tcp (client only)
native ssl_load_cert_into_context(Socket:id, const certificate[], const private_key[]); // certificate & private_key might be the same .pem file
native ssl_shutdown(Socket:id) = socket_destroy;
native ssl_get_peer_certificate(Socket:id, method, subject[], issuer[], remote_clientid = 0xFFFF);
native ssl_set_accept_timeout(Socket:id, interval); // interval in miliseconds
native ssl_set_mode(Socket:id, mode); // see above SSL_modes (enum)


// client & server (udp)
forward onUDPReceiveData(Socket:id, data[], data_len, remote_client_ip[], remote_client_port);
// client only (tcp)
forward onSocketAnswer(Socket:id, data[], data_len); // called when socket_connect() has been used and the server sends data
forward onSocketClose(Socket:id);
// server only (tcp)
forward onSocketReceiveData(Socket:id, remote_clientid, data[], data_len); // called when a remote client sends data
forward onSocketRemoteConnect(Socket:id, remote_client[], remote_clientid); // called when a remote client connects to our socket server
forward onSocketRemoteDisconnect(Socket:id, remote_clientid); // called when a remote client disconnects from our socket server
