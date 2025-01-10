#include "zepch.h"

#include "Zoo.h"

#include <iostream>

#include "Game.h"
#include "Core/Menu.h"

namespace SerengetiZoo
{
    BOOL Zoo::s_isOpen = FALSE;

    wistd::unique_ptr<Zoo> Zoo::Initialize()
    {
        return wistd::unique_ptr<Zoo>(new Zoo());
    }

    // Closes the zoo for the day and tells visitors to leave
    void Zoo::EndTurn()
    {
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("\n%cZoo is closing for the rest of the day...\n\n"), PINK);
        }

        m_canAddNewVisitorsEvent.ResetEvent();
        Close.SetEvent();

        for (auto i = m_visitors.size(); i-- > 0;)
        {
            m_visitors[i]->WaitForThreads();
            RemoveVisitor(m_visitors[i]);
        }

        s_isOpen = FALSE;

        Menu::PrintScore();

        ResetOpenZooTimer();
    }

    void Zoo::FeedAnimals(const int cageNumber) const
    {
        const auto& cage = m_cages[cageNumber];

        if (cage->IsCageEmpty())
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage->GetName());

            return;
        }

        cage->FeedAnimals();
    }

    // Checks all cages and returns if all cages are empty
    bool Zoo::IsZooEmpty() const
    {
        int count = 0;

        for (const auto& cage : m_cages)
        {
            if (cage->IsCageEmpty())
            {
                count++;
            }
        }

        return count == GameSettings::MaxCages;
    }

    // Prints all animals
    void Zoo::GetAllAnimals() const
    {
        for (const auto& cage : m_cages)
        {
            cage->GetAnimals();
        }
    }

    // Prints all animals health values
    void Zoo::GetAllAnimalsHealth() const
    {
        for (const auto& cage : m_cages)
        {
            if (cage->IsCageEmpty())
            {
                auto lock = Renderer::GetConsoleLock().lock();

                cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage->GetName());

                continue;
            }

            cage->GetAnimalsHealth();
        }
    }

    // Prints all animals interactivity values
    void Zoo::GetAllAnimalsInteractivity() const
    {
        for (const auto& cage : m_cages)
        {
            if (cage->IsCageEmpty())
            {
                auto lock = Renderer::GetConsoleLock().lock();

                cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage->GetName());
                continue;
            }

            cage->GetAnimalsInteractiveLevel();
        }
    }

    Cage* Zoo::GetRandomCage() const
    {
        std::vector<Cage*> availableCages;

        for (const auto& cage : m_cages)
        {
            if (!cage->IsCageEmpty())
            {
                availableCages.push_back(cage.get());
            }
        }

        if (availableCages.empty())
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("%cNo available cages found.\n"), RED);

            return nullptr;
        }

        return availableCages.at(rand() % availableCages.size());
    }

    // Prints the visitors currently in the zoo with their location and values
    void Zoo::GetAllVisitors() const
    {
        if (m_visitors.empty())
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("%cCurrently no visitors...\n"), PINK);
        }

        for (const auto& visitor : m_visitors)
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(
                _T("[%c%d%r] %s is %s "),
                SKYBLUE,
                visitor->GetCurrentCage(),
                visitor->GetName(),
                visitor->GetStatus());

            if (visitor->GetHappiness() < 5)
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), PINK, visitor->GetHappiness());
            }
            else if (visitor->GetHappiness() <= 7)
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, visitor->GetHappiness());
            }
            else
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), LIME, visitor->GetHappiness());
            }
        }
    }

    // Increase happiness level for anyone in the specified cage
    void Zoo::ShowCaseAnimals(const int cageNumber)
    {
        auto guard = m_cs.lock();

        for (const auto& visitor : m_visitors)
        {
            if (visitor->GetCurrentCage() == cageNumber)
            {
                if (m_cages.at(cageNumber)->IsCageEmpty())
                {
                    continue;
                }

                visitor->AddHappiness();
            }
        }
    }

    void Zoo::WaitForThreads() const
    {
        WaitForSingleObject(m_addVisitorsThread.get(), INFINITE);
        WaitForSingleObject(m_zooTimerThread.get(), INFINITE);

        m_significantEvent->WaitForThread();

        for (const auto& cage : m_cages)
        {
            cage->WaitForThreads();
        }
    }

    Zoo::Zoo() : m_canAddNewVisitorsEvent(wil::EventOptions::ManualReset)
    {
        m_cages.reserve(GameSettings::MaxCages);

        for (int i = 0; i < GameSettings::MaxCages; ++i)
        {
            m_cages.emplace_back(wil::make_unique_failfast<Cage>(i + 1));
        }

        m_newVisitorsTimer.reset(CreateWaitableTimer(nullptr, true, nullptr));
        THROW_LAST_ERROR_IF(!m_newVisitorsTimer.is_valid());

        m_addVisitorsThread.reset(CreateThread(nullptr, 0, AddVisitorsThread, this, 0, nullptr));

        s_isOpen = TRUE;

        m_canAddNewVisitorsEvent.SetEvent();
        ResetAddVisitorsEvent();

        m_zooTimerThread.reset(CreateThread(nullptr, 0, OpenZooTimerThread, this, 0, nullptr));

        m_significantEvent = wil::make_unique_failfast<SignificantEvent>();
        m_significantEvent->AddListener([this]() { OnSignificantEvent(); });
    }

    void Zoo::OnSignificantEvent()
    {
        std::random_device generator;
        std::uniform_int_distribution cageDist(0, static_cast<int>(m_cages.size()) - 1);
        std::bernoulli_distribution boolDist;

        Cage* randomCage;

        do
        {
            randomCage = m_cages.at(cageDist(generator)).get();
        }
        while (randomCage->IsCageEmpty());

        Animal* selectedAnimal;

        do
        {
            selectedAnimal = randomCage->GetRandomAnimal();
        }
        while (selectedAnimal == nullptr);

        if (boolDist(generator)) {
            {
                auto lock = Renderer::GetConsoleLock().lock();

                cwl::WriteLine(_T("\n%c%s the %s has escaped!\n\n"), YELLOW, selectedAnimal->GetName(), selectedAnimal->GetType());
                cwl::WriteLine(_T("You have %clost%r %d points because all visitors left the zoo...\n"), PINK, static_cast<DWORD>(m_visitors.size()));
            }

            Game::UpdateScore(-(static_cast<int>(m_visitors.size())));

            randomCage->RemoveAnimal(*selectedAnimal);
        }
        else {
            {
                auto lock = Renderer::GetConsoleLock().lock();

                cwl::WriteLine(_T("\n%c%s the %s has given birth to a baby %s!\n\n"), LIME, selectedAnimal->GetName(), selectedAnimal->GetType(), selectedAnimal->GetType());
                cwl::WriteLine(_T("All visitors have left for the day and you %cearned%r %d points...\n"), LIME, 3 * static_cast<DWORD>(m_visitors.size()));
            }

            Game::UpdateScore(3 * static_cast<DWORD>(m_visitors.size()));

            randomCage->AddAnimal();
        }

        m_significantEvent->ResetTimer();

        EndTurn();
    }

    // Removes a visitor from the visitor list and returns the unique name
    void Zoo::RemoveVisitor(const wistd::unique_ptr<Visitor>& visitor)
    {
        auto guard = m_cs.lock();

        const auto iterator = std::ranges::find(m_visitors, visitor);

        if (iterator == m_visitors.end()) { return; }

        Helpers::AddRandomName(visitor->GetName());
        m_visitors.erase(iterator);
    }

    // Resets the timer that adds users in the add visitors thread
    void Zoo::ResetAddVisitorsEvent()
    {
        std::random_device generator;
        std::uniform_int_distribution range(5, 30);

        m_addVisitorsEventTime.QuadPart = -(range(generator) * TimerSeconds);

        THROW_LAST_ERROR_IF(!SetWaitableTimer(m_newVisitorsTimer.get(), &m_addVisitorsEventTime, 0, nullptr, nullptr, false));
    }

    // Resets the timer that triggers the Zoo opening
    void Zoo::ResetOpenZooTimer()
    {
        m_closedEventTime.QuadPart = -(30 * TimerSeconds);
        THROW_LAST_ERROR_IF(!SetWaitableTimer(Open.get(), &m_closedEventTime, 0, nullptr, nullptr, false));
    }

    DWORD WINAPI Zoo::OpenZooTimerThread(const LPVOID lpParam)
    {
        const auto zoo = static_cast<Zoo*>(lpParam);

        HANDLE events[2];
        events[0] = Open.get();
        events[1] = Game::EndGame.get();

        do
        {
            if (WaitForMultipleObjects(_countof(events), events, false, INFINITE) == 0)
            {
                Close.ResetEvent();

                s_isOpen = TRUE;

                auto lock = Renderer::GetConsoleLock().lock();

                cwl::WriteLine(_T("\n%c------------------------------------\n"), LIME);
                cwl::WriteLine(_T("%cThe Zoo is now open.\n"), LIME);
                cwl::WriteLine(_T("%c------------------------------------\n\n"), LIME);

                zoo->m_canAddNewVisitorsEvent.SetEvent();
                zoo->ResetAddVisitorsEvent();
            }
            else
            {
                return 0;
            }
        }
        while (TRUE);
    }

    DWORD WINAPI Zoo::AddVisitorsThread(const LPVOID lpParam)
    {
        const auto zoo = static_cast<Zoo*>(lpParam);

        HANDLE exitEvents[2] { };

        exitEvents[0] = zoo->m_canAddNewVisitorsEvent.get();
        exitEvents[1] = Game::EndGame.get();

        HANDLE events[3] { };

        events[0] = zoo->m_newVisitorsTimer.get();
        events[1] = Close.get();
        events[2] = Game::EndGame.get();

        do
        {
            const auto waitForZooOpen = WaitForMultipleObjects(_countof(exitEvents), exitEvents, false, INFINITE);

            if (waitForZooOpen == 1) { return 0; }

            const auto wait = WaitForMultipleObjects(_countof(events), events, false, INFINITE);

            if (wait == 1)
            {
                continue;
            }

            if (wait == 2)
            {
                return 0;
            }

            std::random_device generator;
            std::uniform_int_distribution range(1, 4);

            const int visitorsToAdd = range(generator);

            for (int i = 0; i != visitorsToAdd; ++i)
            {
                zoo->m_visitors.emplace_back(wil::make_unique_failfast<Visitor>(zoo->m_cages));
            }

            zoo->ResetAddVisitorsEvent();
        }
        while (true);
    }
}
