//
// Created by krahmalx on 24.05.2026.
//

#ifndef LOGLIB

#define LOGLIB
#include <time.h>

#define LOGLIB_VERSION_ "1.1"

typedef enum { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_CRITICAL } log_level_t;

struct log_event_t {
    log_level_t level;
    const char  *logger_name;       // имя логгера или NULL
    const char  *msg;               // уже сгенерированное сообщение (после sprintf-подстановок)
    const char  *file;              // файл (опционально)
    int         line;               // номер строки (опционально)
    const char  *thread_name;       // или thread id (опционально)
    time_t      timestamp;          // секунды
    long        timestamp_us;       // микросекунды или миллисекунды
    const void  *user_ctx;          // для расширений
};

typedef int (*log_formatter_fn)(const struct log_event_t *evt, char *out_buf, size_t capacity);

// escaping
size_t escape_text(const char *in, char *out, size_t cap);
size_t json_escape(const char *in, char *out, size_t cap);

// formating
int txt_formatter(const struct log_event_t *evt, char *out_buf, size_t capacity);
int json_formatter(const struct log_event_t *evt, char *out_buf, size_t capacity);

void set_formatter(log_formatter_fn f);
void set_format(char fmt[]);
char get_format();

// logging
void emit(const struct log_event_t* evt);

void slog(char msg[], int level, char logger_name[]);
void flog(const char msg[], log_level_t level, const char logger_name[], char fmt[], const char file[], int line, const char thread_name[], const char user_ctx[]);

#endif //LOGLIB

