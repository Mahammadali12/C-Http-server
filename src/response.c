#include "response.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>

static const char *status_reason(int code) {
    switch (code) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        default:  return "Internal Server Error";
    }
}

void send_error(int client_fd, int status_code) {
    char header[512];
    int n = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, status_reason(status_code));
    send(client_fd, header, n, 0);
}

void send_file_response(int client_fd, const char *filepath, const http_request_t *req) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        send_error(client_fd, 404);
        return;
    }
    struct stat st;
    fstat(fd, &st);

    char header[1024];
    const char *date = get_current_time();
    int n = snprintf(header, sizeof(header),
        "%s 200 %s\r\n"
        "Date: %s\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: %s\r\n"
        "Connection: close\r\n"
        "\r\n",
        req->version, status_reason(200),
        date,
        (size_t)st.st_size,
        get_mime_type(filepath)
    );
    send(client_fd, header, n, 0);

    //send body
    char buf[4096];
    ssize_t r;
    while ((r = read(fd, buf, sizeof(buf))) > 0) {
        send(client_fd, buf, r, 0);
    }
    close(fd);
}

void send_response(int client_fd, const http_request_t *req) {
    // Map URI to file
    char path[512] = "resources";
    strncat(path, req->uri, sizeof(path)-strlen(path)-1);
    if (req->uri[strlen(req->uri)-1] == '/')
        strncat(path, "index.html", sizeof(path)-strlen(path)-1);
    send_file_response(client_fd, path, req);
}
