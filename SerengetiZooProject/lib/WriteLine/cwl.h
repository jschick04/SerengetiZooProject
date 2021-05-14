#ifndef OUTPUT_H
#define OUTPUT_H

#ifdef UNICODE
#define WriteLine WriteLineW
#else
#define WriteLine WriteLineA
#endif

#include <Windows.h>

class __declspec(dllexport) cwl {
    public:
    static int WriteLineW(LPCWSTR pszText, ...);
    static int WriteLineA(LPCSTR pszText, ...);

    private:
    static DWORD PrintToFile(HANDLE hOutputFile, LPSTR pszText);
};

#endif
