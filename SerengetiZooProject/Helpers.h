#pragma once

#include <array>
#include <wil/resource.h>
#include <Windows.h>
#include "Animal.h"

class Helpers {
public:
    static int GetRandomInteractiveLevel();
    static void AddRandomName(LPCTSTR name);
    static LPCTSTR GetRandomName();

    static LPCTSTR AnimalTypeToString(AnimalType type);
private:
    static wil::critical_section m_cs;
    static std::array<LPCTSTR, 53> m_uniqueNames;
};
