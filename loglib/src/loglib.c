//
// Created by krahmalx on 24.05.2026.
//
#include <time.h>
#include <stdio.h>
#include "loglib.h"

static const char *level_to_str(const log_level_t l) {
    switch(l)
    {
        case LOG_DEBUG:    return "DEBUG";
        case LOG_INFO:     return "INFO";
        case LOG_WARN:     return "WARN";
        case LOG_ERROR:    return "ERROR";
        case LOG_CRITICAL: return "CRITICAL";
        default:           return "UNKNOWN";
    }
}

static char* format = "[%s.%03ld] [%s] %s: %s (%s:%d)";

char get_format()
{
    const char *fmt_temp = format;
    return *fmt_temp;
}

void set_format(char fmt[])
{
    format = fmt;
}

int txt_formatter(const struct log_event_t *evt, char *out_buf, const size_t capacity) {
    struct tm tm;
    localtime_r(&evt->timestamp, &tm);

    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm);

    // Формируем строку: "0000-00-00 12:34:56.123 [INFO] logger: message (file:line)"
    int written = snprintf(out_buf, capacity, format,
        ts, evt->timestamp_us / 1000,
        level_to_str(evt->level),
        evt->logger_name ? evt->logger_name : "root",
        evt->msg ? evt->msg : "",
        evt->file ? evt->file : "?",
        evt->line);

    if (written < 0)
        return -1;

    if ((size_t)written >= capacity)
        return (int)written;

    return written;
}

int json_formatter(const struct log_event_t *evt, char *out_buf, size_t capacity) {
    char ts[32];
    struct tm tm;

    localtime_r(&evt->timestamp, &tm);
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", &tm);

    int off = snprintf(out_buf, capacity, "{\"ts\":\"%s.%03ld\",\"lvl\":\"%s\",\"logger\":\"",
    ts, evt->timestamp_us/1000, level_to_str(evt->level));

    if (off < 0)
        return -1;

    if ((size_t)off >= capacity)
        return off;

    size_t rem = capacity - (size_t)off;
    size_t wrote = json_escape(evt->logger_name ? evt->logger_name : "root", out_buf+off, rem);
    off += (int)wrote;

    if ((size_t)off >= capacity)
        return (int)off;

    int n = snprintf(out_buf+off, capacity-off, "\",\"msg\":\"");

    if (n < 0)
        return -1;

    off += n;

    if ((size_t)off >= capacity)
        return off;

    rem = capacity - (size_t)off;

    wrote = json_escape(evt->msg ? evt->msg : "", out_buf+off, rem);
    off += (int)wrote;


    if ((size_t)off >= capacity)
        return (int)off;

    n = snprintf(out_buf+off, capacity-off, "\"}");

    if (n < 0)
        return -1;

    off += n;

    return off;
}