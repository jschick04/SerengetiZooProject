#pragma once

namespace SerengetiZoo
{
    class Helpers
    {
    public:
        static void AddRandomName(LPCTSTR name);
        static LPCTSTR GetRandomName();

    private:
        static inline wil::critical_section s_cs;
        static std::array<LPCTSTR, 51> s_uniqueNames;
    };
}
