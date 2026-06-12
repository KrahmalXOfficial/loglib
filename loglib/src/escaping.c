//
// Created by krahmalx on 24.05.2026.
//
#include <stdio.h>
#include "../include/escaping.h"

size_t escape_text(const char *in, char *out, size_t cap) {
    size_t needed = 0;
    size_t i = 0;
    while (*in) {
        unsigned char c = (unsigned char)*in++;
        if (c == '\n') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]='n'; }
        } else if (c == '\r') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]='r'; }
        } else if (c == '\t') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]='t'; }
        } else if (c >= 0x20 && c <= 0x7E) {
            needed += 1; if (i + 1 < cap) out[i++] = c;
        } else {
            needed += 4; if (i + 4 < cap) {
                static const char hex[] = "0123456789ABCDEF";
                out[i++] = '\\';
                out[i++] = 'x';
                out[i++] = hex[c >> 4];
                out[i++] = hex[c & 0xF];
            }
        }
    }
    if (cap > 0) {
        if (i < cap) out[i] = '\0';
        else out[cap-1] = '\0';
    }
    return needed;
}

static const char *hex = "0123456789abcdef";

size_t json_escape(const char *in, char *out, size_t cap) {
    size_t needed = 0;
    size_t i = 0;

    while (*in) {
        unsigned char c = (unsigned char)*in++;
        if (c == '\"' || c == '\\' || c == '/') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]= (char)c; }
        } else if (c == '\b') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]='b'; }
        } else if (c == '\f') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]='f'; }
        } else if (c == '\n') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]='n'; }
        } else if (c == '\r') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]='r'; }
        } else if (c == '\t') {
            needed += 2; if (i + 2 < cap) { out[i++]='\\'; out[i++]='t'; }
        } else if (c < 0x20) {
            // \u00XX
            needed += 6;
            if (i + 6 < cap) {
                out[i++]='\\'; out[i++]='u'; out[i++]='0'; out[i++]='0';
                out[i++]=hex[(c>>4)&0xF]; out[i++]=hex[c&0xF];
            }
        } else {
            needed += 1;
            if (i + 1 < cap) {
                out[i++] = (char)c;
            }
        }
    }
    if (cap > 0) {
        if (i < cap) out[i] = '\0'; else out[cap-1]='\0';
    }
    return needed;
}

