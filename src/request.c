#include "request.h"
#include <string.h>
#include <stdio.h>

int parse_http_request(const char *buf, http_request_t *req) {
    // 1. Split request line
    if (sscanf(buf, "%15s %255s %15s", req->method, req->uri, req->version) != 3)
        return -1;

    // 2. Advance past first line
    const char *p = strstr(buf, "\r\n");
    if (!p) return -1;
    p += 2;

    // 3. Parse headers until blank line
    req->header_count = 0;
    while (*p && strncmp(p, "\r\n", 2) != 0 && req->header_count < MAX_HEADERS) {
        char name[MAX_HEADER_NAME], value[MAX_HEADER_VALUE];
        if (sscanf(p, "%63[^:]: %255[^\r\n]", name, value) == 2) {
            strcpy(req->headers[req->header_count].name, name);
            strcpy(req->headers[req->header_count].value, value);
            req->header_count++;
        }
        p = strstr(p, "\r\n");
        if (!p) break;
        p += 2;
    }
    return 0;
}

const char* get_header(const http_request_t *req, const char *name) {
    for (int i = 0; i < req->header_count; i++) {
        if (strcasecmp(req->headers[i].name, name) == 0)
            return req->headers[i].value;
    }
    return NULL;
}