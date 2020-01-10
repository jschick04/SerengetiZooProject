#pragma once

#include <wil/resource.h>
#include <Windows.h>

class Helpers {
public:
    static int GetRandomInteractiveLevel();
    static void AddRandomName(LPCTSTR name);
    static LPCTSTR GetRandomName();

private:
    static wil::critical_section m_cs;

    LPCTSTR m_uniqueNames[];

    //static std::vector<LPCTSTR> m_uniqueNames;
};
