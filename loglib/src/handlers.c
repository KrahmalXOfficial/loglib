//
// Created by krahmalx on 24.05.2026.
//
#include "../include/handlers.h"

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

#define LOG_STACK_BUF 1024    // prefered stack buffer size
#define LOG_MAX_BUF   2500   // hard limit for allocation

static log_formatter_fn current_formatter = formatter;
static struct file_handler CURRENT_FILE_HANDLER;
static int lineno = 0;

static FILE *log_out = NULL;

static void now_ts(time_t *sec_out, long *usec_out) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        *sec_out = ts.tv_sec;
        *usec_out = ts.tv_nsec / 1000; // ns -> us
    } else {
        *sec_out = 0; *usec_out = 0;
    }
}

void set_formatter(const log_formatter_fn f) {
    current_formatter = f;
}

void set_log_out(FILE *out) {
    log_out = out;
}


int new_file_handler(struct file_handler *handler, const char *filename) {
    handler->file = fopen(filename, "a");
    if (handler->file == NULL) {
        fprintf(stderr, "ERROR: Could not open file '%s'\n", filename);
        return 1;
    }

    return 0;
}


void set_current_file(const char file_name[]) {
    if (file_name != NULL || CURRENT_FILE_HANDLER.file == NULL) {
        new_file_handler(&CURRENT_FILE_HANDLER, file_name);
    } else {
        flog(ERROR, "FileHandler", "File handler already has a file open");
    }
}

void slog(const int level, char logger_name[], char *msg, ...) { flog(level, logger_name, msg); }

void flog(log_level_t level, const char *logger, const char *fmt, ...) {
    char stack_buf[LOG_STACK_BUF];
    char *buf = stack_buf;
    size_t cap = sizeof(stack_buf);
    va_list ap;

    /* First pass: try vsnprintf into stack buffer */
    va_start(ap, fmt);
    int needed = vsnprintf(buf, cap, fmt, ap);
    va_end(ap);

    if (needed < 0)
        fprintf(stderr, "ERROR: Could not format log message '%s'\n", fmt); /* encoding error */

    if ((size_t)needed >= cap) {
        /* need bigger buffer */
        size_t want = (size_t)needed + 1;
        if (want > LOG_MAX_BUF)
            want = LOG_MAX_BUF;

        buf = malloc(want);

        if (!buf)
            return;

        cap = want;

        va_start(ap, fmt);
        int needed2 = vsnprintf(buf, cap, fmt, ap);
        va_end(ap);

        if (needed2 < 0) {
            free(buf == stack_buf ? NULL : buf);
            return;
        }
        /* If still truncated (needed2 >= cap), we keep truncated result */
    }

    struct log_event evt;
    evt.level = level;
    evt.logger_name = logger;
    evt.msg = buf;
    evt.file = CURRENT_FILE_HANDLER.file;
    evt.line = lineno;
    now_ts(&evt.timestamp, &evt.timestamp_us);
    localtime_r(&evt.timestamp, &evt.tm);

    if (current_formatter) {
        char outbuf[1024];
        escape_text(evt.msg, outbuf, sizeof(outbuf));
        char* r = formatter(get_format()->msg_format, &evt.level, &evt.tm, &evt.timestamp_us, logger, outbuf);

        if (r != NULL && log_out) {
            fputs(r, log_out);
            fputc('\n', log_out);
            fflush(log_out);

        } else if (log_out) {
            /* formatter signalled needed size or error; write raw message fallback */
            fputs(buf, log_out);
            fputc('\n', log_out);
            fflush(log_out);
        }

        if (r != NULL)
            free(r);

    } else if (log_out) {
        /* default fallback: timestamp + level + message */
        char ts[32];
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &evt.tm);
        fprintf(log_out, "%s.%03f [%d] %s: %s\n", ts, 0.1f, (int)level, logger?: "root", buf);
        fflush(log_out);
    }

    if (buf != stack_buf)
        free(buf);
}