#ifndef WRITE_LINE_H
#define WRITE_LINE_H

#include <Windows.h>

#ifdef UNICODE
#define ConsoleWriteLine ConsoleWriteLineW
#else
#define ConsoleWriteLine ConsoleWriteLineA
#endif

int ConsoleWriteLine(LPTSTR pszText, ...);

#endif
