#include "hooks.h"
#include "pattern_scan.h"
#include "types.h"
#include "joaat.h"
#include "bypass.h"
#include "log.h"
#include <MinHook.h>

get_hash2_t      g_orig_hash2 = NULL;
http_req_t       g_orig_http = NULL;
get_thread_ctx_t g_orig_thread_ctx = NULL;
player_join_t    g_orig_join = NULL;

static void* resolve_target(void* addr) {
    uint8_t* p = (uint8_t*)addr;
    while (*p == 0xE9) {
        int32_t rel = *(int32_t*)(p + 1);
        p = p + 5 + rel;
    }
    return p;
}

static bool str_contains(const char* haystack, const char* needle) {
    if (!haystack || !needle) return false;
    for (const char* h = haystack; *h; h++) {
        const char* hh = h;
        const char* n = needle;
        while (*n && *hh == *n) { hh++; n++; }
        if (*n == 0) return true;
    }
    return false;
}

static uint32_t hook_get_hash(void) {
    return 0x124EA49Du;
}

static uint32_t hook_get_hash2(void* ac_var, uint32_t seed) {
    char* ctx = (char*)(*g_targets.ctx);
    bool prev = ctx ? *(bool*)(ctx + CTX_BATTLEYE_ENABLED) : false;
    if (ctx) *(bool*)(ctx + CTX_BATTLEYE_ENABLED) = true;
    uint32_t ret = g_orig_hash2(ac_var, seed);
    if (ctx) *(bool*)(ctx + CTX_BATTLEYE_ENABLED) = prev;
    return ret;
}

static void hook_skel_update(void* skeleton, int type) {
    char* skel = (char*)skeleton;
    SkelUpdateMode* mode = *(SkelUpdateMode**)(skel + SKEL_UPDATE_MODES);
    uint32_t common_main = joaat_lower("Common Main");
    for (; mode; mode = mode->next) {
        if (mode->type != type) continue;
        SkelUpdateBase* group = mode->head;
        for (; group; group = group->next) {
            if (group->hash != common_main) {
                skel_run(group);
                continue;
            }
            SkelUpdateGroup* grp = (SkelUpdateGroup*)group;
            SkelUpdateBase* item = grp->head;
            for (; item; item = item->next) {
                if (item->hash != HASH_TAMPER_ACTIONS
                    && item->hash != joaat_lower("TamperActions")) {
                    skel_run(item);
                }
            }
        }
        break;
    }
}

static bool hook_metric(void* a, void* b, void* c, void* d) {
    (void)a; (void)b; (void)c; (void)d;
    return false;
}

static void hook_http_req(void* request) {
    uintptr_t req = (uintptr_t)request;
    const char** host = (const char**)(req + 0x18F8);
    const char* path = *(const char**)(req + 0x1900);
    if (path && str_contains(path, "GameTransactions.asmx/Bonus")) {
        *host = "0.0.0.0";
    }
    g_orig_http(request);
}

static BOOL hook_thread_ctx(HANDLE thread, LPCONTEXT ctx) {
    BOOL ret = g_orig_thread_ctx(thread, ctx);
    ctx->Dr0 = 0;
    ctx->Dr1 = 0;
    ctx->Dr2 = 0;
    ctx->Dr3 = 0;
    return ret;
}

static bool hook_player_join(void* iface, void* context) {
    if (!g_be_active) {
        if (*(bool*)((char*)context + PCTX_IS_LOCAL)) {
            return true;
        }
    }
    return g_orig_join(iface, context);
}

bool be_hooks_install(void) {
    if (MH_Initialize() != MH_OK) {
        BE_LOG("[be] mh init failed\n");
        return false;
    }

    void* target;
    MH_STATUS s;

#define HOOK(name, detour, orig) \
    target = resolve_target(g_targets.name); \
    s = MH_CreateHook(target, detour, (void**)orig); \
    if (s != MH_OK) { BE_LOG("[be] hook " #name " failed\n"); return false; }

    HOOK(get_hash,   hook_get_hash,   NULL);
    HOOK(get_hash2,  hook_get_hash2,  (void**)&g_orig_hash2);
    HOOK(skeleton_update, hook_skel_update, NULL);
    HOOK(metric,     hook_metric,     NULL);
    HOOK(http_req,   hook_http_req,   (void**)&g_orig_http);
    HOOK(player_join, hook_player_join, (void**)&g_orig_join);

#undef HOOK

    s = MH_CreateHookApi(L"kernel32.dll", "GetThreadContext", hook_thread_ctx, (void**)&g_orig_thread_ctx);
    if (s != MH_OK) { BE_LOG("[be] hook GetThreadContext failed\n"); return false; }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        BE_LOG("[be] enable hooks failed\n");
        return false;
    }

    BE_LOG("[be] hooks installed\n");
    return true;
}

void be_hooks_remove(void) {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
