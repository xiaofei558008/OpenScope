#ifndef OS_NETBAR_H
#define OS_NETBAR_H
#include <windows.h>
/* UI P2: top-most network / remote bar (layout-spec v2, 26px). */
typedef enum { NB_OFF = 0, NB_OK, NB_SLOW, NB_LOST } OS_NetBarState;
HWND os_netbar_create(HWND parent);
HWND os_netbar_hwnd(void);
int  os_netbar_height(void);                 /* DPI-scaled 26px, 0 if not created */
/* role: L"LOCAL" / L"REMOTE"; peer: host:port or NULL; rtt_ms < 0 = unknown; lock: input-lock owner or NULL */
void os_netbar_set(OS_NetBarState st, const wchar_t* role, const wchar_t* peer, int rtt_ms, const wchar_t* lock);
const wchar_t* os_netbar_summary(void);      /* short text for status bar */
#endif