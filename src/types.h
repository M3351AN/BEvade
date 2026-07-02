#ifndef BEVADE_TYPES_H
#define BEVADE_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

typedef struct Obf32 {
    uint32_t unk1;
    uint32_t unk2;
    uint32_t unk3;
    uint32_t unk4;
} Obf32;

static inline uint32_t obf32_get(Obf32* o) {
    return (o->unk1 & o->unk4) | (o->unk2 & ~o->unk4);
}

static inline void obf32_set(Obf32* o, uint32_t val) {
    uint32_t seed = (uint32_t)(uintptr_t)GetTickCount();
    o->unk3 = seed;
    seed = (uint32_t)(uintptr_t)GetTickCount();
    o->unk4 = seed;
    o->unk1 = seed & val;
    o->unk2 = val & ~seed;
}

#define CTX_BATTLEYE_ENABLED   0xA91u
#define PCTX_IS_LOCAL          0x41u
#define SKEL_UPDATE_MODES      0x140u
#define HASH_TAMPER_ACTIONS    0xA0F39FB6u

typedef struct SkelUpdateBase {
    void** vtable;
    uint64_t pad;
    uint32_t hash;
    struct SkelUpdateBase* next;
} SkelUpdateBase;

typedef struct SkelUpdateGroup {
    SkelUpdateBase base;
    SkelUpdateBase* head;
} SkelUpdateGroup;

typedef struct SkelUpdateMode {
    int type;
    SkelUpdateBase* head;
    struct SkelUpdateMode* next;
} SkelUpdateMode;

static inline void skel_run(SkelUpdateBase* item) {
    typedef void (*RunFn)(void*);
    RunFn run = (RunFn)(item->vtable[1]);
    run(item);
}

#endif
