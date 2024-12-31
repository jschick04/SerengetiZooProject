#include "szpch.h"

#include "Application.h"

namespace SerengetiZoo
{
    Application::Application(const ApplicationSpecification& specification) : m_specification(specification)
    {
        m_game = Game::Initialize();
    }

    void Application::Run()
    {
        m_game->OnRender();
    }
}
