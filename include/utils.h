#ifndef UTILS_H
#define UTILS_H

// Return a static string with the current time in RFC 1123 format
const char *get_current_time(void);


#define LOG(level, fmt, ...) \
    fprintf(stderr, "[%s] [%5s] [pid=%d] " fmt "\n", \
            get_current_time(), level, getpid(), ##__VA_ARGS__)

// Return MIME type based on file extension
const char *get_mime_type(const char *path);

#endif // UTILS_H
