#pragma once

#include "Entities/Zoo.h"

namespace SerengetiZoo
{
    class Game
    {
    public:
        static wistd::unique_ptr<Game> Initialize();

        void OnRender() const;

    public:
        static wil::unique_event_failfast EndGame;

    private:
        explicit Game();

    private:
        uint32_t m_score = 0;
        wistd::unique_ptr<Zoo> m_zoo;
    };
}
