#pragma once

#include "Entities/Zoo.h"

namespace SerengetiZoo
{
    class Game
    {
    public:
        static wistd::unique_ptr<Game> Initialize();

        static DWORD GetScore() { return s_score; };

        void OnRender() const;

    public:
        static wil::unique_event_failfast EndGame;

    private:
        explicit Game();

    private:
        static DWORD s_score;
        wistd::unique_ptr<Zoo> m_zoo;
    };
}
