#pragma once

#include <array>
#include <wil/resource.h>
#include <Windows.h>

enum class VisitorStatus;
enum class AnimalType;

class Helpers {
public:
    static void AddRandomName(LPCTSTR name);
    static LPCTSTR GetRandomName();

    static LPCTSTR AnimalTypeToString(AnimalType type);
    static LPCTSTR VisitorStatusToString(VisitorStatus type);
private:
    static wil::critical_section m_cs;
    static std::array<LPCTSTR, 53> m_uniqueNames;
};
