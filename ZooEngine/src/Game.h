#pragma once

#include "Entities/Zoo.h"

namespace SerengetiZoo
{
    class Game
    {
    public:
        static wistd::unique_ptr<Game> Initialize();

        static DWORD GetScore() { return s_score; }

        void OnRender() const;

    public:
        static inline wil::unique_event_failfast EndGame = wil::unique_event_failfast(wil::EventOptions::ManualReset);

    private:
        explicit Game();

    private:
        static inline DWORD s_score = 0;
        wistd::unique_ptr<Zoo> m_zoo;
    };
}
