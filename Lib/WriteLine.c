#include <Windows.h>
#include <tchar.h>
#include <stdarg.h>
#include "WriteLine.h"
#include "ConsoleColors.h"

CRITICAL_SECTION g_consoleCS;
HANDLE g_Console;
WORD g_Attributes;

DWORD PrintToFile(HANDLE hOutputFile, const LPTSTR pszText) {
    const byte stringBytes = (byte)(_tcslen(pszText) * sizeof(TCHAR));
    DWORD bytesWritten = 0;

    if (WriteFile(hOutputFile, pszText, stringBytes, &bytesWritten, NULL)) {
        return bytesWritten;
    }

    // TODO: Return failure to GetLastError

    return 0;
}

BOOL InitializeConsole() {
    if (g_Console == NULL) {
        if (!InitializeCriticalSectionAndSpinCount(&g_consoleCS, 4000)) {
            return FALSE;
        }

        g_Console = GetStdHandle(STD_OUTPUT_HANDLE);

        if (g_Console == INVALID_HANDLE_VALUE) {
            return FALSE;
        }

        if (g_Attributes == 0) {
            CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;

            if (!GetConsoleScreenBufferInfo(g_Console, &consoleBufferInfo)) {
                return FALSE;
            }

            g_Attributes = consoleBufferInfo.wAttributes;
        }
    }

    return TRUE;
}

int ConsoleWriteLine(const LPTSTR pszText, ...) {
    TCHAR buffer[_MAX_U64TOSTR_BASE2_COUNT];
    va_list args;

    if (!InitializeConsole()) {
        ExitProcess(-1);
    }

    EnterCriticalSection(&g_consoleCS);

    va_start(args, pszText);

    for (int i = 0; i < (int)(_tcslen(pszText)); i++) {
        if (pszText[i] == '%') {
            i++;
            switch (pszText[i]) {
                case 'c' :
                    SetConsoleTextAttribute(g_Console, va_arg(args, WORD));
                    break;
                case 'd' :
                    _itot_s(va_arg(args, int), buffer, _countof(buffer), 10);
                    PrintToFile(g_Console, buffer);
                    break;
                case 'r' :
                    SetConsoleTextAttribute(g_Console, g_Attributes);
                    break;
                case 's' :
                    PrintToFile(g_Console, va_arg(args, LPTSTR));
                    break;
            }
        } else {
            TCHAR letter[2] = {pszText[i], '\0'};
            PrintToFile(g_Console, letter);
        }
    }

    va_end(args);

    SetConsoleTextAttribute(g_Console, g_Attributes);

    LeaveCriticalSection(&g_consoleCS);

    //CloseHandle(console);

    return 0;
}

int ConsoleWriteError(const LPTSTR pszText) {
    return ConsoleWriteLine(_T("%cERROR: %s"), RED, pszText);
}
