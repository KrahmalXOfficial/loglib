//
// Created by alex on 26.05.2026.
//

#ifndef LOGLIB_FORMATING_H
#define LOGLIB_FORMATING_H
#include <stdio.h>
#include <time.h>

// etc
typedef enum { DEBUG, INFO, WARN, ERROR, CRITICAL } log_level_t;

struct log_event {
    log_level_t level;
    const char  *logger_name;       // logger name or NULL
    const char  *msg;               // message
    FILE        *file;              // file (optional)
    int         line;               // lineno (optional)
    const char  *thread_name;       // thread name or thread id (optional)
    time_t      timestamp;          // sec
    long        timestamp_us;       // us
    const void  *user_ctx;          // user context
    char*       msg_format;         // message fmt
    char*       date_format;        // date fmt
    struct tm   tm;                 // tm
};

struct log_format {
    char*       msg_format;         // message fmt
    char*       date_format;        // date fmt
};

typedef char* (*log_formatter_fn)       (const char* format, const log_level_t* level, const struct tm* tm, const long* timestamp, const char* logger, const char* message);
typedef int (*log_format_fn)            (const struct log_event *lf);
typedef struct log_format log_format_t;

// formating
char* formatter     (const char* format, const log_level_t* level, const struct tm* tm, const long* timestamp, const char* logger, const char* message);

void set_formatter          (log_formatter_fn f);
void set_format             (char* msg_fmt, ...);

struct log_format* get_format();

#endif //LOGLIB_FORMATING_H
