#ifndef BEVADE_JOAAT_H
#define BEVADE_JOAAT_H

#include <stdint.h>

static inline uint32_t joaat_lower(const char* str) {
    uint32_t hash = 0;
    while (*str) {
        char c = *str++;
        if (c >= 'A' && c <= 'Z') c = (char)(c | 0x20);
        hash += (uint32_t)(unsigned char)c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

#endif
