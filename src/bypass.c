#include "bypass.h"
#include "pattern_scan.h"
#include "hooks.h"
#include "types.h"
#include "log.h"
#include <winternl.h>

volatile bool g_running = true;
bool g_be_active = false;
bool g_fsl = false;
bool g_fsl_bypass = false;

typedef struct _ldr_entry {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    void* DllBase;
    void* EntryPoint;
    unsigned long SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
} ldr_entry;

typedef bool (*fsl_bypass_fn)(void);

static bool unicode_eq_ascii_ci(const UNICODE_STRING* u, const char* a) {
    size_t n = 0;
    while (a[n]) n++;
    if (u->Length != n * sizeof(wchar_t)) return false;
    for (size_t i = 0; i < n; i++) {
        wchar_t wc = u->Buffer[i];
        unsigned char ac = (unsigned char)a[i];
        if (wc >= L'A' && wc <= L'Z') wc |= 0x20;
        if (ac >= 'A' && ac <= 'Z') ac |= 0x20;
        if (wc != (wchar_t)ac) return false;
    }
    return true;
}

static void detect_fsl(void) {
    int winmm_count = 0;
    PPEB peb = (PPEB)(NtCurrentTeb()->ProcessEnvironmentBlock);
    if (!peb || !peb->Ldr) return;
    LIST_ENTRY* head = &peb->Ldr->InMemoryOrderModuleList;
    LIST_ENTRY* e = head->Flink;
    while (e != head) {
        ldr_entry* entry = CONTAINING_RECORD(e, ldr_entry, InMemoryOrderLinks);
        if (entry->BaseDllName.Buffer && unicode_eq_ascii_ci(&entry->BaseDllName, "WINMM.dll")) {
            winmm_count++;
            HMODULE h = (HMODULE)entry->DllBase;
            if (h) {
                fsl_bypass_fn fn = (fsl_bypass_fn)GetProcAddress(h, "LawnchairIsProvidingBattlEyeBypass");
                if (fn && fn()) g_fsl_bypass = true;
            }
        }
        e = e->Flink;
    }
    g_fsl = (winmm_count > 1);
}

static void init_hash_store(void) {
    if (!*g_targets.hash_store) {
        Obf32* obj = (Obf32*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Obf32));
        if (obj) *g_targets.hash_store = obj;
    }
    if (*g_targets.hash_store) {
        obf32_set(*g_targets.hash_store, 0x124EA49Du);
    }
}

static bool apply_patch(void* addr, const uint8_t* bytes, size_t len) {
    DWORD old;
    if (!VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &old)) return false;
    for (size_t i = 0; i < len; i++) ((uint8_t*)addr)[i] = bytes[i];
    DWORD tmp;
    VirtualProtect(addr, len, old, &tmp);
    return true;
}

static void restore_patch(void* addr, const uint8_t* original, size_t len) {
    DWORD old;
    VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &old);
    for (size_t i = 0; i < len; i++) ((uint8_t*)addr)[i] = original[i];
    DWORD tmp;
    VirtualProtect(addr, len, old, &tmp);
}

DWORD WINAPI be_thread(void* param) {
    (void)param;

    if (!be_scan()) {
        BE_LOG("[be] scan failed\n");
        return 1;
    }

    init_hash_store();
    detect_fsl();

    g_be_active = (*g_targets.restart_code == 0
                   && GetModuleHandleA("BEClient_x64.dll") != NULL
                   && !g_fsl);

    if (!be_hooks_install()) {
        BE_LOG("[be] hook install failed\n");
        return 1;
    }

    bool patched = false;
    uint8_t saved[7];
    if (!g_fsl_bypass && !g_be_active) {
        static const uint8_t bytes[] = {0x48, 0x83, 0xC4, 0x38, 0x5F, 0x5E, 0xC3};
        void* addr = g_targets.patch_addr;
        for (int i = 0; i < 7; i++) saved[i] = ((uint8_t*)addr)[i];
        patched = apply_patch(addr, bytes, 7);
        if (patched) BE_LOG("[be] patch applied\n");
    }

    BE_LOG("[be] loop start\n");
    while (g_running) {
        if (!g_fsl_bypass && !g_be_active) {
            *g_targets.restart_code = 0;
            *g_targets.need_restart = false;
            *g_targets.banned = false;
        }
        Sleep(100);
    }

    if (patched) restore_patch(g_targets.patch_addr, saved, 7);
    be_hooks_remove();
    BE_LOG("[be] stopped\n");
    return 0;
}
