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
cell AMX_NATIVE_CALL n_ssl_create(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_ssl_connect(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_ssl_load_cert(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_ssl_set_mode(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_socket_send_array(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_ssl_set_accept_timeout(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_ssl_init(AMX* amx, cell* params);
cell AMX_NATIVE_CALL n_ssl_get_peer_certificate(AMX* amx, cell* params);