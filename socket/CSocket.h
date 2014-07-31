#pragma once

#include "main.h"

#define MAX_SOCK 10

struct SocketStruct {
	bool tcp;
	bool listen;
	bool success;
	bool is_client; // true = client, false = server
	bool active_thread;
	bool ssl;
	int max_clients;
	int* connected_clients;
	char bind_ip[15];
	SSL_CTX *ssl_context; // client & server
	SSL *ssl_handle; // client side
	SSL **ssl_clients; // server side
};

class CSocket
{
public:
	struct SocketStruct m_pSocketInfo[MAX_SOCK];
	int m_pSocket[MAX_SOCK];

	CSocket();
	~CSocket(); 
	int create_socket(int type);
	int find_free_slot(int* arr, int size);
	int set_max_connections(int socketid, int maxcon);
	int connect_socket(int socketid, char* address, int port);
	int listen_socket(int socketid, int port);
	int stop_listen_socket(int socketid);
	int bind_socket(int socketid, char* ip);
	int destroy_socket(int socketid);
	int close_remote_connection(int socketid, int remote_client_id);
	int send_socket(int socketid, char* data, int len);
	int sendto_socket(int socketid, char* ip, int port, char* data, int len);
	int sendto_remote_client(int socketid, int remote_clientid, char* data);
	int set_nonblocking_socket(int fd);
	int is_remote_client_connected(int socketid, int remote_clientid);
	int is_socket_valid(int socketid);
	char* get_remote_client_ip(int socketid, int remote_clientid);
	void close_socket(int socket);
	int ssl_create(int socketid, int method);
	int ssl_connect(int socketid);
	int ssl_load_cert(int socketid, char* szCert, char* szKey);
	int ssl_set_mode(int socketid, int mode);
	int socket_send_array(int socketid, cell* aData, int size);
	int ssl_set_timeout(int socketid, DWORD dwInterval);
	void ssl_init();
//private:
};

#ifdef WIN32
DWORD socket_connection_thread(void* lpParam);
DWORD socket_receive_thread(void* lpParam);
#else
void* socket_connection_thread(void* lpParam);
void* socket_receive_thread(void* lpParam);
#endif