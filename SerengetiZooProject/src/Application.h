#pragma once

#include "Game.h"

namespace SerengetiZoo
{
    struct ApplicationSpecification { };

    class Application
    {
    public:
        explicit Application(const ApplicationSpecification& specification = ApplicationSpecification());

    public:
        void Run();

    private:
        wistd::unique_ptr<Game> m_game;
        ApplicationSpecification m_specification;
    };

    inline Application* CreateApplication() { return new Application(); };
}
