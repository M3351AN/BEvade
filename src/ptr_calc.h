#ifndef BEVADE_PTR_CALC_H
#define BEVADE_PTR_CALC_H

#include <stdint.h>

static inline uintptr_t ptr_add(uintptr_t ptr, intptr_t off) {
    return ptr + (uintptr_t)off;
}

static inline uintptr_t ptr_sub(uintptr_t ptr, intptr_t off) {
    return ptr - (uintptr_t)off;
}

static inline uintptr_t ptr_rip(uintptr_t ptr) {
    int32_t rel = *(int32_t*)ptr;
    return ptr + (uintptr_t)(int64_t)rel + 4;
}

#endif
