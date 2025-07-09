#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

const char *get_current_time(void) {
    static char buf[128];
    time_t now = time(NULL);
    struct tm tm = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &tm);
    return buf;
}




static struct { const char *ext, *mime; } mime_types[] = {
    {".html", "text/html"},
    {".css",  "text/css"},
    {".js",   "application/javascript"},
    {".json", "application/json"},
    {".png",  "image/png"},
    {".jpg",  "image/jpeg"},
    {".gif",  "image/gif"},
    {NULL,    "application/octet-stream"}
};

const char *get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    for (int i = 0; mime_types[i].ext; i++) {
        if (strcmp(ext, mime_types[i].ext) == 0)
            return mime_types[i].mime;
    }
    return "application/octet-stream";
}
