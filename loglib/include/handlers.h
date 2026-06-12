//
// Created by alex on 26.05.2026.
//

#ifndef LOGLIB_EMITER_H
#define LOGLIB_EMITER_H

#include "formating.h"
#include "escaping.h"

// handlers
struct file_handler {
    FILE *file;
    const char *file_name;
    log_level_t min_level;
};

struct handler {
    char *name;
    log_level_t min_level;
};

// logging
void emit               (const struct log_event* evt);
void slog               (int level, char logger_name[], char *msg, ...);
void flogf              (log_level_t level, const char logger_name[], const char *_format, ...);
void flog               (log_level_t level, const char *logger, const char *fmt, ...);
int new_file_handler    (struct file_handler *handler, const char *filename);
void set_current_file   (const char file_name[]);
void set_log_out        (FILE *out);

#endif //LOGLIB_EMITER_H
