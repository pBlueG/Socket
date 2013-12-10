#pragma once
#include "main.h"

std::list<AMX*> g_pAMX;
logprintf_t logprintf;
extern void *pAMXFunctions;
CSocket* g_pSocket;
std::queue<remoteConnect> onRemoteConnect;
std::queue<remoteDisconnect> onRemoteDisconnect;
std::queue<receiveData> onSocketReceiveData;
std::queue<socketAnswer> onSocketAnswer;
std::queue<socketClose> onSocketClose;
std::queue<socketUDP> onUDPReceiveData;

PLUGIN_EXPORT int PLUGIN_CALL ProcessTick( )
{
	if(!onRemoteConnect.empty()) {
		remoteConnect tempData = onRemoteConnect.front();
		cell amx_Address, amx_Ret, *phys_addr;
		int amx_idx;
		for (std::list<AMX *>::iterator a = g_pAMX.begin(); a != g_pAMX.end(); a++) {
			if(amx_FindPublic(* a, "onSocketRemoteConnect", &amx_idx) == AMX_ERR_NONE) {
				amx_Push(* a, tempData.remote_clientid);
				amx_PushString(* a, &amx_Address, &phys_addr, tempData.remote_client, 0, 0);
				amx_Push(* a, tempData.socketid);
				amx_Exec(* a, &amx_Ret, amx_idx);
				amx_Release(* a, amx_Address);
			}
		}
		free(tempData.remote_client);
		onRemoteConnect.pop();
	}
	if(!onRemoteDisconnect.empty()) {
		remoteDisconnect tempData = onRemoteDisconnect.front();
		cell amx_Ret;
		int amx_idx;
		for (std::list<AMX *>::iterator a = g_pAMX.begin(); a != g_pAMX.end(); a++) {
			if(amx_FindPublic(* a, "onSocketRemoteDisconnect", &amx_idx) == AMX_ERR_NONE) {
				amx_Push(* a, tempData.remote_clientid);
				amx_Push(* a, tempData.socketid);
				amx_Exec(* a, &amx_Ret, amx_idx);
			}
		}
		onRemoteDisconnect.pop();
	}
	if(!onSocketReceiveData.empty()) {
		receiveData tempData = onSocketReceiveData.front();
		cell amx_Address, amx_Ret, *phys_addr;
		int amx_idx;
		for (std::list<AMX *>::iterator a = g_pAMX.begin(); a != g_pAMX.end(); a++) {
			if(amx_FindPublic(* a, "onSocketReceiveData", &amx_idx) == AMX_ERR_NONE) {
				amx_Push(* a, tempData.data_len);
				amx_PushString(* a, &amx_Address, &phys_addr, tempData.data, 0, 0);
				amx_Push(* a, tempData.remote_clientid);
				amx_Push(* a, tempData.socketid);
				amx_Exec(* a, &amx_Ret, amx_idx);
				amx_Release(* a, amx_Address);
			}
		}
		free(tempData.data);
		onSocketReceiveData.pop();
	}
	if(!onSocketAnswer.empty()) {
		socketAnswer tempData = onSocketAnswer.front();
		cell amx_Address, amx_Ret, *phys_addr;
		int amx_idx;
		for (std::list<AMX *>::iterator a = g_pAMX.begin(); a != g_pAMX.end(); a++) {
			if(amx_FindPublic(* a, "onSocketAnswer", &amx_idx) == AMX_ERR_NONE) {
				amx_Push(* a, tempData.data_len);
				amx_PushString(* a, &amx_Address, &phys_addr, tempData.data, 0, 0);
				amx_Push(* a, tempData.socketid);
				amx_Exec(* a, &amx_Ret, amx_idx);
				amx_Release(* a, amx_Address);
			}
		}
		free(tempData.data);
		onSocketAnswer.pop();
	}
	if(!onSocketClose.empty()) {
		socketClose tempData = onSocketClose.front();
		int amx_idx;
		cell amx_Ret;
		for (std::list<AMX *>::iterator a = g_pAMX.begin(); a != g_pAMX.end(); a++) {
			if(amx_FindPublic(* a, "onSocketClose", &amx_idx) == AMX_ERR_NONE) {
				amx_Push(* a, tempData.socketid);
				amx_Exec(* a, &amx_Ret, amx_idx);
			}
		}
		onSocketClose.pop();
	}
	if(!onUDPReceiveData.empty()) {
		socketUDP tempData = onUDPReceiveData.front();
		int amx_idx;
		cell amx_Address[2], amx_Ret;
		for (std::list<AMX *>::iterator a = g_pAMX.begin(); a != g_pAMX.end(); a++) {
			if(amx_FindPublic(* a, "onUDPReceiveData", &amx_idx) == AMX_ERR_NONE) {
				amx_Push(* a, tempData.remote_port);
				amx_PushString(* a, &amx_Address[0], NULL, tempData.remote_ip, 0, 0);
				amx_Push(* a, tempData.data_len);
				amx_PushString(* a, &amx_Address[1], NULL, tempData.data, 0, 0);
				amx_Push(* a, tempData.socketid);
				amx_Exec(* a, &amx_Ret, amx_idx);
				amx_Release(* a, amx_Address[0]);
				amx_Release(* a, amx_Address[1]);
			}
		}
		free(tempData.data);
		free(tempData.remote_ip);
		onUDPReceiveData.pop();
	}
	return 1;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];
	g_pSocket = new CSocket();
    logprintf(" Socket plugin v%s loaded.", VERSION);
    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    logprintf(" Socket plugin successfully unloaded.");
	delete g_pSocket;
}

AMX_NATIVE_INFO PluginNatives[] =
{
	{"socket_create", n_create_socket},
	{"socket_bind", n_bind_socket},
	{"socket_set_max_connections", n_set_max_connections},
	{"socket_connect", n_connect_socket},
	{"socket_listen", n_listen_socket},
	{"socket_stop_listen", n_stop_listen_socket},
	{"socket_destroy", n_destroy_socket},
	{"socket_close_remote_client", n_close_remote_connection},
	{"socket_send", n_send_socket},
	{"socket_sendto_remote_client", n_sendto_remote_client},
	{"socket_remote_client_connected", n_is_remote_client_connected},
	{"is_socket_valid", n_is_socket_valid},
	{"get_remote_client_ip", n_get_remote_client_ip},
    {0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad( AMX *amx )
{
	g_pAMX.push_back(amx);
    return amx_Register(amx, PluginNatives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload( AMX *amx )
{
	for(std::list<AMX *>::iterator i = g_pAMX.begin();i != g_pAMX.end();i++) {
		if(* i == amx) {
			g_pAMX.erase(i);
			break;
		}
	}
    return AMX_ERR_NONE;
}