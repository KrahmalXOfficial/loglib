//
// Created by alex on 26.05.2026.
//

#ifndef LOGLIB_ESCAPING_H
#define LOGLIB_ESCAPING_H

// escaping
size_t escape_text(const char *in, char *out, size_t cap);
size_t json_escape(const char *in, char *out, size_t cap);

#endif //LOGLIB_ESCAPING_H
