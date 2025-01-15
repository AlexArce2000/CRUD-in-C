#include <jansson.h> // La clave para construir el post
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h> // para usar el postgresql
#include <unistd.h>
#include <arpa/inet.h>
#include "headers.h"

#define PORT 8080
#define BUFFER_SIZE 2048
#define QUERY_SIZE 1024



void handle_options(int client_socket) {
    // Enviar encabezados CORS adecuados para solicitudes preflight
    send_response(client_socket, "200 OK", "application/json", "");
}

// Función para enviar la respuesta con encabezados CORS
void send_response(int client_socket, const char *status, const char *content_type, const char *content) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %lu\r\n"
             "Access-Control-Allow-Origin: *\r\n"  // Permite solicitudes de cualquier origen
             "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
             "Access-Control-Allow-Headers: Content-Type, hx-current-url, hx-request, hx-target\r\n" //Especifica los encabezados permitidos que el navegador puede enviar
             "Access-Control-Max-Age: 3600\r\n"  // Opcional: para indicar que el navegador puede almacenar los resultados de la preflight durante 1 hora
             "\r\n%s",
             status, content_type, strlen(content), content);
    write(client_socket, buffer, strlen(buffer));
}

void handle_client(int client_socket) {
    PGconn *conn;
    PGresult *res;
    char buffer[BUFFER_SIZE];
    char query[QUERY_SIZE];
    int bytes_read;

    // Conectar a la base de datos, reemplazar con el usuario, contraseña y nombre de base de datos
    conn = PQconnectdb("user=postgres password=fpuna dbname=crudinc host=localhost");
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        close(client_socket);
        return;
    }

    // Leer la solicitud del cliente
    bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        PQfinish(conn);
        close(client_socket);
        return;
    }
    buffer[bytes_read] = '\0';

    // Mostrar la solicitud para depuración
    printf("Received request:\n%s\n", buffer);

    // Encontrar el cuerpo de la solicitud
    char *body = strstr(buffer, "\r\n\r\n");
    if (body) {
        body += 4; // Saltar los encabezados HTTP
    } else {
        body = ""; // Si no hay cuerpo, manejar como vacío
    }

    // Mostrar el cuerpo extraído para depuración
    printf("Extracted body:\n%s\n", body);
    // Si la solicitud es OPTIONS, manejarla de forma especial
    if (strstr(buffer, "OPTIONS /example") != NULL) {
        handle_options(client_socket);
    } 
    // Manejar solicitudes según el método HTTP
    if (strstr(buffer, "POST /example") != NULL) {
        handle_post(client_socket, conn, body);
    } else if (strstr(buffer, "GET /example") == buffer) {
        handle_get(client_socket, conn);
    } else if (strstr(buffer, "DELETE /example") == buffer) {
        handle_delete(client_socket, conn, buffer);
    } else if (strstr(buffer, "PUT /example") == buffer) {
        handle_put(client_socket, conn, body, buffer);
    } else {
        send_response(client_socket, "400 Bad Request", "application/json", "Bad Request");
    }

    PQfinish(conn);
    close(client_socket);
}

void handle_post(int client_socket, PGconn *conn, const char *body) {
    char query[QUERY_SIZE];
    PGresult *res;

    json_error_t error;
    json_t *root = json_loads(body, 0, &error);
    if (!root) {
        send_response(client_socket, "400 Bad Request", "application/json", "Invalid JSON format");
        return;
    }

    json_t *name_json = json_object_get(root, "name");
    json_t *description_json = json_object_get(root, "description");

    if (!json_is_string(name_json) || !json_is_string(description_json)) {
        send_response(client_socket, "400 Bad Request", "application/json", "Invalid JSON format");
        json_decref(root);
        return;
    }

    const char *name = json_string_value(name_json);
    const char *description = json_string_value(description_json);

    char *escaped_name = PQescapeLiteral(conn, name, strlen(name));
    char *escaped_description = PQescapeLiteral(conn, description, strlen(description));

    snprintf(query, sizeof(query), "INSERT INTO example (name, description) VALUES (%s, %s)",
             escaped_name, escaped_description);
    printf("Executing query: %s\n", query);
    
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Query failed: %s", PQerrorMessage(conn));
        send_response(client_socket, "500 Internal Server Error", "application/json", "Database query failed");
    } else {
        send_response(client_socket, "201 Created", "application/json", body);
    }
    PQclear(res);
    PQfreemem(escaped_name);
    PQfreemem(escaped_description);
    json_decref(root);
}

void handle_get(int client_socket, PGconn *conn) {
    char query[QUERY_SIZE];
    PGresult *res;

    snprintf(query, sizeof(query), "SELECT id, name, description FROM example");
    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        send_response(client_socket, "500 Internal Server Error", "application/json", "Database query failed");
    } else {
        char response_body[BUFFER_SIZE] = "[";
        for (int i = 0; i < PQntuples(res); i++) {
            if (i > 0) strcat(response_body, ",");
            strcat(response_body, "{");
            strcat(response_body, "\"id\":\"");
            strcat(response_body, PQgetvalue(res, i, 0));
            strcat(response_body, "\",");
            strcat(response_body, "\"name\":\"");
            strcat(response_body, PQgetvalue(res, i, 1));
            strcat(response_body, "\",");
            strcat(response_body, "\"description\":\"");
            strcat(response_body, PQgetvalue(res, i, 2));
            strcat(response_body, "\"}");
        }
        strcat(response_body, "]");
        send_response(client_socket, "200 OK", "application/json", response_body);
    }
    PQclear(res);
}

void handle_delete(int client_socket, PGconn *conn, const char *buffer) {
    printf("Request body: %s\n", buffer);
    char query[QUERY_SIZE];
    PGresult *res;
    char *id_start = strstr(buffer, "id=");
    if (id_start) {
        id_start += 3; // Saltar "id="
        char *id_end = strchr(id_start, ' ');
        if (id_end) {
            *id_end = '\0';
            snprintf(query, sizeof(query), "DELETE FROM example WHERE id=%s", id_start);
            printf("Executing query: %s\n", query);

            res = PQexec(conn, query);
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                fprintf(stderr, "Query failed: %s", PQerrorMessage(conn));
                send_response(client_socket, "500 Internal Server Error", "application/json", "Database query failed");
            } else {
                send_response(client_socket, "200 OK", "application/json", "[]");
            }
            PQclear(res);
        } else {
            send_response(client_socket, "400 Bad Request", "application/json", "Invalid ID format");
        }
    } else {
        send_response(client_socket, "400 Bad Request", "application/json", "ID not provided");
    }
}

void handle_put(int client_socket, PGconn *conn, const char *body, const char *buffer) {
    printf("Request buffer:\n%s\n", buffer); // Imprime la solicitud completa
    printf("Request body:\n%s\n", body);    // Imprime el cuerpo de la solicitud

    char query[QUERY_SIZE];
    PGresult *res;
    char *id_start = strstr(buffer, "id=");
    if (id_start) {
        id_start += 3; // Saltar "id="
        char *id_end = strchr(id_start, ' ');
        if (id_end) {
            *id_end = '\0'; // Terminar la cadena del ID
            printf("Extracted ID: %s\n", id_start);  // Imprime la ID extraída

            json_error_t error;
            json_t *root = json_loads(body, 0, &error);
            if (!root) {
                send_response(client_socket, "400 Bad Request", "application/json", "Invalid JSON format");
                return;
            }

            json_t *name_json = json_object_get(root, "name");
            json_t *description_json = json_object_get(root, "description");

            if (!json_is_string(name_json) || !json_is_string(description_json)) {
                send_response(client_socket, "400 Bad Request", "application/json", "Invalid JSON format");
                json_decref(root);
                return;
            }

            const char *name = json_string_value(name_json);
            const char *description = json_string_value(description_json);
            printf("Received name: %s\n", name);  // Imprime el 'name'
            printf("Received description: %s\n", description);  // Imprime el 'description'

            char *escaped_name = PQescapeLiteral(conn, name, strlen(name));
            char *escaped_description = PQescapeLiteral(conn, description, strlen(description));

            snprintf(query, sizeof(query), "UPDATE example SET name=%s, description=%s WHERE id=%s",
                     escaped_name, escaped_description, id_start);
            printf("Executing query: %s\n", query);

            res = PQexec(conn, query);
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                fprintf(stderr, "Query failed: %s", PQerrorMessage(conn));
                send_response(client_socket, "500 Internal Server Error", "application/json", "Database query failed");
            } else {
                send_response(client_socket, "200 OK", "application/json", body);
            }
            PQclear(res);
            PQfreemem(escaped_name);
            PQfreemem(escaped_description);
            json_decref(root);
        } else {
            send_response(client_socket, "400 Bad Request", "application/json", "Invalid ID format");
        }
    } else {
        send_response(client_socket, "400 Bad Request", "application/json", "ID not provided");
    }
}