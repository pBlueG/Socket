#pragma once

#include "main.h"

#define MAX_SOCK 10

struct SocketStruct {
	bool tcp;
	bool listen;
	bool success;
	bool is_client; // true = client, false = server
	bool active_thread;
	int max_clients;
	int* connected_clients;
	char bind_ip[15];
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
	int sendto_remote_client(int socketid, int remote_clientid, char* data);
	int set_nonblocking_socket(int fd);
	int is_remote_client_connected(int socketid, int remote_clientid);
	int is_socket_valid(int socketid);
	char* get_remote_client_ip(int socketid, int remote_clientid);
	void close_socket(int socket);
//private:
};

#ifdef WIN32
DWORD socket_connection_thread(void* lpParam);
DWORD socket_receive_thread(void* lpParam);
#else
void* socket_connection_thread(void* lpParam);
void* socket_receive_thread(void* lpParam);
#endif