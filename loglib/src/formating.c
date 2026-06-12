//
// Created by krahmalx on 24.05.2026.
//

#include "../include/formating.h"
#include "../include/escaping.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_CAP 1024

static const char *level_to_str(const log_level_t l) {
    switch(l)
    {
        case DEBUG:    return "DEBUG";
        case INFO:     return "INFO";
        case WARN:     return "WARN";
        case ERROR:    return "ERROR";
        case CRITICAL: return "CRITICAL";
        default:       return "UNKNOWN";
    }
}

static struct log_format LOG_FMT;

static void now_ts(time_t *sec_out, long *usec_out) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        *sec_out = ts.tv_sec;
        *usec_out = ts.tv_nsec / 1000; // ns -> us
    } else {
        *sec_out = 0; *usec_out = 0;
    }
}

static long get_pattern(const char *pattern, const char *str, const int where)
{
    char *ptr = strstr(str, pattern);

    if (ptr != NULL) {
        ptr += where;

        long id = strtol(ptr, NULL, where);
        printf("Найдено: %ld\n", id);

        return id;
    }

    return 0;
}

/**
 *
 * @param prefix the string that is copied to str
 * @param str just string
 * @return pointer to new string
 * @warning free result using free
 */
static char* prepend(const char* prefix, const char* str) {
    size_t len_prefix = strlen(prefix);
    size_t len_str = strlen(str);

    char *new_str = malloc(len_prefix + len_str + 1);

    if (new_str != NULL) {
        strcpy(new_str, prefix);
        strcat(new_str, str);
    }
    return new_str;
}

void set_format(char* msg_fmt, ...) {
    LOG_FMT.msg_format = msg_fmt;
}

struct log_format* get_format() {
    return &LOG_FMT;
}

/**
 * formats a log line using a template
 *
 * @param format  Format string (e.g. "%YYYY-%M-%d %h:%m:%s [%lvl.1] %rep")
 * @param level   Logging level (string, may be NULL)
 * @param tm      Pointer to struct tm (time, may be NULL)
 * @param timestamp Timestamp (us)
 * @param message Log message (string, may be NULL)
 * @return        A pointer to a dynamically allocated result string, or NULL if error. free using free().
 */
char* formatter(const char* format, const log_level_t* level, const struct tm* tm, const long* timestamp, const char* logger, const char* message) {
    if (!format) return NULL;

    size_t buf_size = 1024;
    char* result = malloc(buf_size);
    if (!result) return NULL;
    size_t result_len = 0;

    const char* p = format;
    while (*p) {
        if (*p != '%') {
            if (result_len + 1 >= buf_size) {
                buf_size *= 2;
                char* new_res = realloc(result, buf_size);

                if (!new_res) {
                    free(result); return NULL;
                }

                result = new_res;
            }
            result[result_len++] = *p;
            p++;
            continue;
        }

        p++;
        if (*p == '%') {
            if (result_len + 1 >= buf_size) {
                buf_size *= 2;
                char* new_res = realloc(result, buf_size);

                if (!new_res) {
                    free(result); return NULL;
                }

                result = new_res;
            }
            result[result_len++] = '%';
            p++;
            continue;
        }

        char name[32] = {0};
        int name_len = 0;
        while (isalpha((unsigned char)*p) && name_len < 31) {
            name[name_len++] = *p;
            p++;
        }
        name[name_len] = '\0';

        if (name_len == 0) {
            if (result_len + 1 >= buf_size) {
                buf_size *= 2;
                char* new_res = realloc(result, buf_size);
                if (!new_res) { free(result); return NULL; }
                result = new_res;
            }
            result[result_len++] = '%';
            if (*p) {
                if (result_len + 1 >= buf_size) {
                    buf_size *= 2;
                    char* new_res = realloc(result, buf_size);
                    if (!new_res) { free(result); return NULL; }
                    result = new_res;
                }
                result[result_len++] = *p;
                p++;
            }
            continue;
        }

        int max_len = 0;
        if (*p == '.') {
            p++;
            while (isdigit((unsigned char)*p)) {
                max_len = max_len * 10 + (*p - '0');
                p++;
            }
            if (isalpha((unsigned char)*p)) {
                p++;
            }
        }

        char value_buf[64] = {0};
        const char* value = NULL;

        if (strcmp(name, "lvl") == 0) {
            value = level_to_str(*level);

        } else if (strcmp(name, "rep") == 0) {
            value = message ? message : "";

        } else if (strcmp(name, "msg") == 0) {
            value = message ? message : "";

        } else if (strcmp(name, "name") == 0) {
            value = logger ? logger : "root";

        } else if (strcmp(name, "YYYY") == 0) {
            if (tm) {
                snprintf(value_buf, sizeof(value_buf), "%04d", tm->tm_year + 1900);
                value = value_buf;
            } else value = "";

        } else if (strcmp(name, "YY") == 0) {
            if (tm) {
                snprintf(value_buf, sizeof(value_buf), "%02d", (tm->tm_year + 1900) % 100);
                value = value_buf;
            } else value = "";

        } else if (strcmp(name, "M") == 0) {
            if (tm) {
                snprintf(value_buf, sizeof(value_buf), "%02d", tm->tm_mon + 1);
                value = value_buf;
            } else value = "";

        } else if (strcmp(name, "d") == 0) {
            if (tm) {
                snprintf(value_buf, sizeof(value_buf), "%02d", tm->tm_mday);
                value = value_buf;
            } else value = "";

        } else if (strcmp(name, "h") == 0) {
            if (tm) {
                snprintf(value_buf, sizeof(value_buf), "%02d", tm->tm_hour);
                value = value_buf;
            } else value = "";

        } else if (strcmp(name, "m") == 0) {
            if (tm) {
                snprintf(value_buf, sizeof(value_buf), "%02d", tm->tm_min);
                value = value_buf;
            } else value = "";

        } else if (strcmp(name, "s") == 0) {
            if (tm) {
                snprintf(value_buf, sizeof(value_buf), "%02d", tm->tm_sec);
                value = value_buf;
            } else value = "";

        } else if (strcmp(name, "ms") == 0) {
            if (tm) {
                snprintf(value_buf, sizeof(value_buf), "%03ld", *timestamp);
                value = value_buf;
            } else value = "";

        } else {
            char unknown[64];
            snprintf(unknown, sizeof(unknown), "%%%s", name);
            value = unknown;
        }

        char truncated[256] = {0};
        if (max_len > 0 && value) {
            size_t vlen = strlen(value);
            if (vlen > (size_t)max_len) {
                strncpy(truncated, value, max_len);
                truncated[max_len] = '\0';
                value = truncated;
            }
        }

        size_t vlen = strlen(value);
        while (result_len + vlen + 1 >= buf_size) {
            buf_size *= 2;
            char* new_res = realloc(result, buf_size);
            if (!new_res) { free(result); return NULL; }
            result = new_res;
        }
        strcpy(result + result_len, value);
        result_len += vlen;
    }

    if (result_len + 1 >= buf_size) {
        buf_size++;
        char* new_res = realloc(result, buf_size);
        if (!new_res) { free(result); return NULL; }
        result = new_res;
    }
    result[result_len] = '\0';

    return result;
}

