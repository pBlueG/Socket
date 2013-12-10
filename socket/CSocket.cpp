#pragma once

#include "CSocket.h"

CThread* g_pThread;
extern CSocket* g_pSocket;
extern logprintf_t logprintf;

CSocket::CSocket()
{
	// c'tor
	g_pThread = new CThread();
	std::fill(m_pSocket, m_pSocket+(sizeof(m_pSocket)/4), -1);
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0 || LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		logprintf("Winsock failed to initialize: %d\n", WSAGetLastError());
		this->~CSocket();
	}
#endif
}

CSocket::~CSocket()
{
	// d'tor
#ifdef WIN32
	WSACleanup();
#endif
	for(int i = 0;i < MAX_SOCK;i++) {
		if(m_pSocket[i] != (-1)) {
			destroy_socket(i);
		}
	}
	//g_pThread->Kill(m_ThreadHandle[]);
}

int CSocket::create_socket(int type)
{
	int iSlot = find_free_slot(m_pSocket, (sizeof(m_pSocket)/4));
	if(type == SOCK_STREAM)
		m_pSocketInfo[iSlot].tcp = true;
	else
		m_pSocketInfo[iSlot].tcp = false;
	m_pSocket[iSlot] = socket(AF_INET, type, 0);
	if(m_pSocket[iSlot] == -1) {
		logprintf("socket_create(): Failed.");
		return (-1);
	}
	m_pSocketInfo[iSlot].success = true; // initiates that the socket has been successfully created
	return iSlot;
}

int CSocket::find_free_slot(int* arr, int size)
{
	for(int i = 0;i < size;i++) {
		if(arr[i] == (-1)) {
			return i;
		}
	}
	return 0xFFFF;
}

int CSocket::set_max_connections(int socketid, int maxcon)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("set_max_connections(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	//m_pSocketInfo[socketid].max_clients = new int[maxcon];
	m_pSocketInfo[socketid].connected_clients = (int*)malloc(sizeof(int)*maxcon);
	std::fill(m_pSocketInfo[socketid].connected_clients, m_pSocketInfo[socketid].connected_clients+maxcon, -1);
	m_pSocketInfo[socketid].max_clients = maxcon;
	return 1;
}

int CSocket::connect_socket(int socketid, char* address, int port)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("socket_connect(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	struct sockaddr_in addr;
	struct hostent *host;
	host = gethostbyname(address);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = *((struct in_addr *) host->h_addr);
	if(strlen(m_pSocketInfo[socketid].bind_ip) > 0) {
		sockaddr_in bind_addr;
		bind_addr.sin_addr.s_addr = inet_addr(m_pSocketInfo[socketid].bind_ip);
		bind_addr.sin_family = AF_INET;
		if(bind(m_pSocket[socketid], (struct sockaddr *)&bind_addr, sizeof(bind_addr)) == -1) {
			logprintf("socket_connect(): Socket ID %d has failed to bind IP %s.", socketid, m_pSocketInfo[socketid].bind_ip);
			return 0;
		}
	} 
	if(connect(m_pSocket[socketid], (struct sockaddr *)&addr, sizeof(sockaddr)) == SOCKET_ERROR) {
		logprintf("socket_connect(): Socket ID %d has failed to connect.", socketid);
		return 0;
	}
	set_nonblocking_socket(m_pSocket[socketid]);
	m_pSocketInfo[socketid].is_client = true; // that way the thread knows the socket will act as a client
	m_pSocketInfo[socketid].active_thread = true;
	g_pThread->Start(socket_receive_thread, (void*)socketid);
	return 1;
}

int CSocket::listen_socket(int socketid, int port)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("socket_listen(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	memset(&(addr.sin_zero), 0, 8);
	if(!strlen(m_pSocketInfo[socketid].bind_ip)) {
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		addr.sin_addr.s_addr = inet_addr(m_pSocketInfo[socketid].bind_ip);
	}
	if(bind(m_pSocket[socketid], (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		logprintf("socket_listen(): Socket has failed to bind on port %d.", port);
		return 0;
	}
	if(m_pSocketInfo[socketid].tcp) { 
		if(listen(m_pSocket[socketid], 10) == -1) {
			logprintf("socket_listen(): Socket has failed to listen on port %d.", port);
			return 0;
		}
	}
	m_pSocketInfo[socketid].is_client = false; // that way the thread knows the socket will act as a server
	m_pSocketInfo[socketid].listen = true;
	m_pSocketInfo[socketid].active_thread = true;
	if(m_pSocketInfo[socketid].tcp)
		g_pThread->Start(socket_connection_thread, (void*)socketid);
	g_pThread->Start(socket_receive_thread, (void*)socketid);
	return 1;
}

int CSocket::stop_listen_socket(int socketid)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("stop_listen_socket(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	close_socket(m_pSocket[socketid]);
	m_pSocketInfo[socketid].listen = false;
	m_pSocketInfo[socketid].active_thread = false;
	return 1;
}

int CSocket::bind_socket(int socketid, char* ip)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("socket_bind(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	strcpy(m_pSocketInfo[socketid].bind_ip, ip);
	return 1;
}

int CSocket::destroy_socket(int socketid)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("socket_destroy(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	if(m_pSocket[socketid] != (-1)) {
		m_pSocketInfo[socketid].active_thread = false;
		if(!m_pSocketInfo[socketid].is_client) {
			for(int i = 0;i < m_pSocketInfo[socketid].max_clients;i++) {
				if(m_pSocketInfo[socketid].connected_clients[i] != (-1)) {
					close_socket(m_pSocketInfo[socketid].connected_clients[i]);
				}
			}
			free(m_pSocketInfo[socketid].connected_clients);
		}
		close_socket(m_pSocket[socketid]);
		m_pSocket[socketid] = (-1);
		m_pSocketInfo[socketid].listen = false;
	}
	//g_pThread->Kill(m_pSocketInfo[socketid].con);
	return 1;
}

int CSocket::close_remote_connection(int socketid, int remote_client_id)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("close_remote_connection(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	int *rem_client = &m_pSocketInfo[socketid].connected_clients[remote_client_id];
	if(*rem_client != (-1)) {
		//shutdown(*rem_client, SD_BOTH);
		close_socket(*rem_client);
		*rem_client = (-1);
	}
	return 1;
}

int CSocket::send_socket(int socketid, char* data, int len)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("socket_send(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	//strcat(data, "\r\n");
	return send(m_pSocket[socketid], data, len, 0);
}

int CSocket::sendto_remote_client(int socketid, int remote_clientid, char* data)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("sendto_remote_client(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	if(m_pSocketInfo[socketid].connected_clients[remote_clientid] != (-1))
		return send(m_pSocketInfo[socketid].connected_clients[remote_clientid], data, strlen(data), 0);
	return 0;
}

int CSocket::is_remote_client_connected(int socketid, int remote_clientid)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("is_remote_client_connected(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	char tempbuf[1024];
	if(remote_clientid <= m_pSocketInfo[socketid].max_clients && m_pSocketInfo[socketid].connected_clients[remote_clientid] != (-1)) {
		if(recv(m_pSocketInfo[socketid].connected_clients[remote_clientid], tempbuf, sizeof(tempbuf), NULL) != 0)
			return 1;
	}
	return 0;
}

// (c) Bjorn Reese 
int CSocket::set_nonblocking_socket(int fd)
{
    DWORD flags;
#ifdef WIN32
    flags = 1;
    return ioctlsocket(fd, FIONBIO, &flags);
#else
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
}  

int CSocket::is_socket_valid(int socketid)
{
	if(socketid > MAX_SOCK || socketid < 0)
		return 0;
	return ((m_pSocket[socketid] != -1) ? 1 : 0);
}

char* CSocket::get_remote_client_ip(int socketid, int remote_clientid)
{
	if(m_pSocket[socketid] == -1 || !m_pSocketInfo[socketid].success) {
		logprintf("get_remote_client_ip(): Socket ID %d doesn't exist or hasn't been created yet", socketid);
		return 0;
	}
	struct sockaddr_in peer_addr;
#ifdef WIN32
	int cLen = sizeof(peer_addr);
#else
	size_t cLen = sizeof(peer_addr);
#endif
	getpeername(m_pSocketInfo[socketid].connected_clients[remote_clientid], (struct sockaddr *)&peer_addr, &cLen);
	return inet_ntoa(peer_addr.sin_addr);
}

void CSocket::close_socket(int socket)
{
#ifdef WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}

#ifdef WIN32
DWORD socket_connection_thread(void* lpParam)
#else
void* socket_connection_thread(void* lpParam)
#endif
{
	int sockID = (int)lpParam;
	sockaddr_in remote_client;
#ifdef WIN32
	int cLen = sizeof(remote_client);
#else
	size_t cLen = sizeof(remote_client);
#endif
	do {
		if(g_pSocket->m_pSocketInfo[sockID].listen) {
			int client = accept(g_pSocket->m_pSocket[sockID], (sockaddr*)&remote_client, &cLen);
			int slot = g_pSocket->find_free_slot(g_pSocket->m_pSocketInfo[sockID].connected_clients, g_pSocket->m_pSocketInfo[sockID].max_clients);
			if(client != NULL && client != SOCKET_ERROR && slot != 0xFFFF) {
				g_pSocket->set_nonblocking_socket(client);
				g_pSocket->m_pSocketInfo[sockID].connected_clients[slot] = client;
				remoteConnect pData;
				pData.remote_client = (char*)malloc(sizeof(char*)*15);
				strcpy(pData.remote_client, inet_ntoa(remote_client.sin_addr));
				pData.remote_clientid = slot;
				pData.socketid = sockID;
				onRemoteConnect.push(pData);
			} else {
				if(slot == 0xFFFF) {
					send(client, "-- Server is full", 17, 0);
					logprintf("onSocketRemoteFail(Socket: %d)\r\n -- Maximum connection limit exceeded. (Limit is %d)\r\n -- Consider using 'socket_set_max_connections()' to increase the limit", sockID, g_pSocket->m_pSocketInfo[sockID].max_clients);
				}
				g_pSocket->close_socket(client);
				/*
					onSocketRemoteFail(Socket:id, remote_client[], remote_clientid)
				*/
			}
		}
		SLEEP(500);
	} while(g_pSocket->m_pSocketInfo[sockID].active_thread);
#ifdef WIN32
	return 1;
#endif
}

#ifdef WIN32
DWORD socket_receive_thread(void* lpParam)
#else
void* socket_receive_thread(void* lpParam)
#endif
{
	int sockID = (int)lpParam,
		curConnection,
		maxClients;
	bool sockType = g_pSocket->m_pSocketInfo[sockID].is_client;
	bool tcpProtocol = g_pSocket->m_pSocketInfo[sockID].tcp;
	char szRecBuffer[1024];
	curConnection = g_pSocket->m_pSocket[sockID];
	// logprintf("Thread started with socket id %d", sockID);
	memset(szRecBuffer, '\0', sizeof(szRecBuffer));
	if(!sockType) maxClients = g_pSocket->m_pSocketInfo[sockID].max_clients;
	do {
		if(!tcpProtocol) {
			// udp
			struct sockaddr_in remote_client;
#ifdef WIN32
			int client_len = sizeof(remote_client);
#else
			size_t client_len = sizeof(remote_client);
#endif
			int byte_len = recvfrom(curConnection, szRecBuffer, sizeof(szRecBuffer), 0, (struct sockaddr*)&remote_client, &client_len);
			if(byte_len > 0) {
				socketUDP pData;
				pData.data = (char*)malloc(sizeof(char)*byte_len);
				strcpy(pData.data, szRecBuffer);
				pData.data_len = byte_len;
				pData.remote_ip = (char*)malloc(sizeof(char*)*15);
				strcpy(pData.remote_ip, inet_ntoa(remote_client.sin_addr));
				pData.remote_port = ntohs(remote_client.sin_port);
				pData.socketid = sockID;
				onUDPReceiveData.push(pData);
				memset(szRecBuffer, '\0', sizeof(szRecBuffer));
			}
			SLEEP(200);
			continue; // skip further processing 
		}
		if(sockType) {
			// tcp client 
			int byte_len = recv(curConnection, szRecBuffer, sizeof(szRecBuffer), NULL);
			if(byte_len > 0) {
				//remove_newline(szRecBuffer);
				szRecBuffer[byte_len] = '\0';
				socketAnswer pData;
				pData.socketid = sockID;
				pData.data_len = byte_len;
				pData.data = (char*)malloc(sizeof(char*)*byte_len);
				strcpy(pData.data, szRecBuffer);
				onSocketAnswer.push(pData);
				memset(szRecBuffer, '\0', sizeof(szRecBuffer));
			}
			if(!byte_len) {
				socketClose pData;
				pData.socketid = sockID;
				onSocketClose.push(pData);
				g_pSocket->destroy_socket(sockID);
			}
		} else { 
			// tcp server
			for(int i = 0;i < maxClients;i++) { // loop through all connected clients
				curConnection = g_pSocket->m_pSocketInfo[sockID].connected_clients[i];
				if(curConnection != (-1)) {
					int byte_len = recv(curConnection, szRecBuffer, sizeof(szRecBuffer), NULL);
					if(byte_len > 0) {
						//remove_newline(szRecBuffer);
						szRecBuffer[byte_len] = '\0';
						receiveData pData;
						pData.data = (char*)malloc(sizeof(char*)*byte_len);
						strcpy(pData.data, szRecBuffer);
						pData.socketid = sockID;
						pData.remote_clientid = i;
						pData.data_len = byte_len;
						onSocketReceiveData.push(pData);
					}
					if(!byte_len) {
						remoteDisconnect pData;
						pData.remote_clientid = i;
						pData.socketid = sockID;
						g_pSocket->close_socket(curConnection);
						g_pSocket->m_pSocketInfo[sockID].connected_clients[i] = (-1); // connection has dropped
						onRemoteDisconnect.push(pData);
					}
				}
			}
		}
		SLEEP(200);
	} while(g_pSocket->m_pSocketInfo[sockID].active_thread);
#ifdef WIN32
	return 1;
#endif
}