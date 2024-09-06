#include <jansson.h>
#include <libpq-fe.h>
void handle_post(int client_socket, PGconn *conn, const char *body);
void handle_get(int client_socket, PGconn *conn);
void handle_delete(int client_socket, PGconn *conn, const char *buffer);
void handle_put(int client_socket, PGconn *conn, const char *body, const char *buffer);
void send_response(int client_socket, const char *status, const char *content_type, const char *content);
void handle_client(int client_socket);