#ifndef REQUEST_H
#define REQUEST_H

#define MAX_HEADERS 32
#define MAX_HEADER_NAME 64
#define MAX_HEADER_VALUE 256

// Represents a single header name/value pair
typedef struct {
    char name[MAX_HEADER_NAME];
    char value[MAX_HEADER_VALUE];
} header_t;

// Represents a full HTTP request
typedef struct {
    char method[16];
    char uri[256];
    char version[16];
    header_t headers[MAX_HEADERS];
    int header_count;
} http_request_t;

// Parse raw `buffer` into `http_request_t`, return 0 on success
int parse_http_request(const char *buffer, http_request_t *req);

// Helper: get header by name (case-insensitive), NULL if missing
const char* get_header(const http_request_t *req, const char *name);

#endif // REQUEST_H