#pragma once

#include "main.h"

cell AMX_NATIVE_CALL n_create_socket(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_bind_socket(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_set_max_connections(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_connect_socket(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_listen_socket(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_stop_listen_socket(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_destroy_socket(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_close_remote_connection(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_send_socket(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_sendto_remote_client(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_is_remote_client_connected(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_is_socket_valid(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_get_remote_client_ip(AMX* amx, cell* params);