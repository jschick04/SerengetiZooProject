#pragma once

namespace SerengetiZoo
{
    class Renderer
    {
    public:
        static wil::critical_section& GetConsoleLock() { return s_cs; }

    private:
        static inline wil::critical_section s_cs;
    };
}
