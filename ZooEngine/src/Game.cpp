#include "zepch.h"

#include "Game.h"

#include "Core/Menu.h"

namespace SerengetiZoo
{
    wistd::unique_ptr<Game> Game::Initialize()
    {
        return wistd::unique_ptr<Game>(new Game());
    }

    void Game::OnRender() const
    {
        do
        {
            Menu::PrintCurrentZooStatus();
            Menu::PrintOptions();

            TCHAR buffer[MAX_PATH];

            int selectedMenuNumber;
            int selectedCageNumber;

            _fgetts(buffer, _countof(buffer), stdin);
            if (_stscanf_s(buffer, _T("%d"), &selectedMenuNumber) != 1)
            {
                auto lock = Renderer::GetConsoleLock().lock();

                cwl::WriteLine(_T("Invalid Selection...\n"));

                continue;
            }

            switch (selectedMenuNumber)
            {
                case 1: // Feed Animals
                    {
                        auto lock = Renderer::GetConsoleLock().lock();

                        cwl::WriteLine(_T("\n%cYou selected - Feed Animals\n\n"), SKYBLUE);
                    }

                    m_zoo->GetAllAnimalsHealth();

                    if (m_zoo->IsZooEmpty()) { break; }

                    {
                        auto lock = Renderer::GetConsoleLock().lock();

                        cwl::WriteLine(_T("\nWhich cage number would you like to feed?\n"));
                    }

                    _fgetts(buffer, _countof(buffer), stdin);
                    if (_stscanf_s(buffer, _T("%d"), &selectedCageNumber) != 1 ||
                        selectedCageNumber < 1 ||
                        selectedCageNumber > GameSettings::MaxCages)
                    {
                        auto lock = Renderer::GetConsoleLock().lock();

                        cwl::WriteLine(_T("Invalid Selection...\n"));
                    }
                    else
                    {
                        m_zoo->FeedAnimals(--selectedCageNumber);
                    }

                    break;
                case 2: // Check Animal Interactivity Levels
                    {
                        auto lock = Renderer::GetConsoleLock().lock();

                        cwl::WriteLine(_T("\n%cYou selected - Check Animal Interactivity Levels\n\n"), SKYBLUE);
                    }

                    m_zoo->GetAllAnimalsInteractivity();

                    break;
                case 3: // Show Case Animal
                    {
                        auto lock = Renderer::GetConsoleLock().lock();

                        cwl::WriteLine(_T("\n%cYou selected - Show Case Animal\n\n"), SKYBLUE);

                        cwl::WriteLine(_T("Which cage number would you like to showcase?\n"));
                    }

                    _fgetts(buffer, _countof(buffer), stdin);
                    if (_stscanf_s(buffer, _T("%d"), &selectedCageNumber) != 1 ||
                        selectedCageNumber < 1 ||
                        selectedCageNumber > GameSettings::MaxCages)
                    {
                        auto lock = Renderer::GetConsoleLock().lock();

                        cwl::WriteLine(_T("Invalid Selection...\n"));
                    }
                    else
                    {
                        m_zoo->ShowCaseAnimals(--selectedCageNumber);
                    }

                    break;
                case 4: // Check Visitors Happiness Level
                    {
                        auto lock = Renderer::GetConsoleLock().lock();
                        
                        cwl::WriteLine(_T("\n%cYou selected - Check Visitors Happiness Level\n\n"), SKYBLUE);
                    }

                    m_zoo->GetAllVisitors();

                    break;
                case 5: // Close the zoo for the day
                    m_zoo->EndTurn();

                    break;
                case 0:
                    {
                        auto lock = Renderer::GetConsoleLock().lock();

                        cwl::WriteLine(_T("\n%cYou selected - Quit\n\n"), SKYBLUE);
                    }

                    m_zoo->EndTurn();

                    EndGame.SetEvent();

                    m_zoo->WaitForThreads();

                    return;
                default:
                    {
                        auto lock = Renderer::GetConsoleLock().lock();

                        cwl::WriteLine(_T("Invalid Selection...\n"));
                    }

                    break;
            }

            Sleep(100);
        }
        while (true);
    }

    Game::Game()
    {
        s_score = 0;
        EndGame.ResetEvent();
        m_zoo = Zoo::Initialize();
    }
}
