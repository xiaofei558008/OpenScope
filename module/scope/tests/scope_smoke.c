/*
 * OpenScope scope.dll module smoke test (no hardware, no UI interaction).
 * Loads the module, checks capability/window type, creates a window,
 * pushes synthetic samples, destroys window, deinits.
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "module_api.h"

typedef const OS_Module* (*os_module_get_fn)(void);

static int check(int cond, const char* what)
{
    printf("%s %s\n", cond ? "PASS" : "FAIL", what);
    return cond ? 0 : 1;
}

int main(void)
{
    int fails = 0;
    HMODULE h;
    os_module_get_fn get;
    const OS_Module* m;
    void* ctx = NULL;
    HWND parent = NULL, win = NULL;
    OS_Sample smp;

    setvbuf(stdout, NULL, _IONBF, 0);
    h = LoadLibraryA("dll\\scope.dll");
    if (!h) {
        printf("FAIL load scope.dll err=%lu\n", GetLastError());
        return 1;
    }
    get = (os_module_get_fn)GetProcAddress(h, "os_module_get");
    if (!get) { printf("FAIL os_module_get\n"); return 1; }
    m = get();
    fails += check(m != NULL && m->api_version == OS_API_VERSION, "module api version");
    fails += check(m != NULL && (m->capabilities & OS_CAP_WINDOW) != 0,
                   "capabilities include OS_CAP_WINDOW");
    fails += check(m != NULL && m->name[0] && strcmp(m->name, "scope") == 0,
                   "module name is scope");
    fails += check(m != NULL && m->window_types &&
                   m->window_types[0].type &&
                   strcmp(m->window_types[0].type, "scope.bar") == 0,
                   "window type scope.bar advertised");

    fails += check(m->init(NULL, &ctx) == OS_ERR_OK, "init returns OK");

    parent = CreateWindowExW(0, L"STATIC", L"parent", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
                             NULL, NULL, GetModuleHandleW(NULL), NULL);
    fails += check(parent != NULL, "parent window created");

    win = m->create_window(ctx, "scope.bar", parent, 0, 0, 300, 200, "t");
    fails += check(win != NULL, "create_window returns HWND");
    fails += check(win != NULL && m->create_window(ctx, "nope", parent, 0, 0, 10, 10, "x") == NULL,
                   "unknown window type rejected");

    /* synthetic samples must not crash (no series yet) */
    memset(&smp, 0, sizeof(smp));
    smp.ts_us = 1000;
    smp.var_id = 0;
    smp.value = 1.5;
    m->on_samples(ctx, &smp, 1);
    m->on_reload(ctx);
    fails += check(1, "on_samples/on_reload no-crash");

    m->destroy_window(ctx, win);
    DestroyWindow(parent);
    m->deinit(ctx);
    FreeLibrary(h);

    printf(fails == 0 ? "ALL PASS\n" : "FAILURES: %d\n", fails);
    return fails == 0 ? 0 : 1;
}
