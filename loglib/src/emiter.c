//
// Created by krahmalx on 24.05.2026.
//
#include "loglib.h"
#include <stdio.h>

static log_formatter_fn current_formatter = txt_formatter;

void set_formatter(const log_formatter_fn f)
{
    current_formatter = f;
}

void emit(const struct log_event_t* evt)
{
    char buf[1024];

    // escaping
    escape_text(evt->msg, buf, sizeof(buf));

    // formating
    current_formatter(evt, buf, sizeof(buf));

    fputs(buf, stdout);
    fputc('\n', stdout);
}

static void now_ts(time_t *sec_out, long *usec_out) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        *sec_out = ts.tv_sec;
        *usec_out = ts.tv_nsec / 1000; // ns -> us
    } else {
        *sec_out = 0; *usec_out = 0;
    }
}

void flog(
    const char msg[],
    const log_level_t level,
    const char logger_name[],
    char fmt[],
    const char file[],
    const int line,
    const char thread_name[],
    const char user_ctx[]
    )
{
    struct log_event_t p;

    time_t sec;
    long us;

    now_ts(&sec, &us);

    p.level         = level;
    p.msg           = msg;
    p.logger_name   = logger_name;
    p.timestamp     = sec;
    p.timestamp_us  = us;
    p.file          = file;
    p.line          = line;
    p.thread_name   = thread_name;
    p.user_ctx      = user_ctx;

    emit(&p);
}

void slog(char msg[], int level, char logger_name[])
{
    flog(msg,level,logger_name,"[%s.%03ld] [%s] %s: %s (%s:%d)","",0,"","");
}