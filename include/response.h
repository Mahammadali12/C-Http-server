#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"

// Send a 200 OK response with a file's contents
void send_file_response(int client_fd, const char *filepath, const http_request_t *req);

// Send a generic error response (e.g., 400, 404)
void send_error(int client_fd, int status_code);

// High‑level dispatch: examine req->uri and call send_file_response or send_error
void send_response(int client_fd, const http_request_t *req);

#endif // RESPONSE_H
