#include "zepch.h"

#include "Zoo.h"

#include "Game.h"
#include "Core/Menu.h"

namespace SerengetiZoo
{
    std::vector<Cage> Zoo::s_cages(GameSettings::MaxCages);
    BOOL Zoo::s_isOpen = FALSE;
    wil::unique_event_failfast Zoo::Close(wil::EventOptions::ManualReset);
    wil::unique_handle Zoo::Open(CreateWaitableTimer(nullptr, false, nullptr));

    wistd::unique_ptr<Zoo> Zoo::Initialize()
    {
        return wistd::unique_ptr<Zoo>(new Zoo());
    }

    // Closes the zoo for the day and tells visitors to leave
    void Zoo::EndTurn()
    {
        cwl::WriteLine(_T("\n%cZoo is closing for the rest of the day...\n\n"), PINK);

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

    void Zoo::FeedAnimals(const int cageNumber)
    {
        if (s_cages[cageNumber - 1].IsCageEmpty())
        {
            cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, s_cages[cageNumber - 1].GetName());

            return;
        }

        s_cages[cageNumber - 1].FeedAnimals();
    }

    // Checks all cages and returns if all cages are empty
    bool Zoo::IsZooEmpty()
    {
        int count = 0;

        for (auto& cage : s_cages)
        {
            if (cage.IsCageEmpty())
            {
                count++;
            }
        }

        return count == GameSettings::MaxCages;
    }

    // Prints all animals
    void Zoo::GetAllAnimals()
    {
        for (auto& cage : s_cages)
        {
            cage.GetAnimals();
        }
    }

    // Prints all animals health values
    void Zoo::GetAllAnimalsHealth()
    {
        for (auto& cage : s_cages)
        {
            if (cage.IsCageEmpty())
            {
                cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage.GetName());

                continue;
            }

            cage.GetAnimalsHealth();
        }
    }

    // Prints all animals interactivity values
    void Zoo::GetAllAnimalsInteractivity()
    {
        for (auto& cage : s_cages)
        {
            if (cage.IsCageEmpty())
            {
                cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage.GetName());
                continue;
            }

            cage.GetAnimalsInteractiveLevel();
        }
    }

    Cage* Zoo::GetRandomCage()
    {
        std::vector<Cage*> availableCages;

        for (auto& cage : s_cages)
        {
            if (!cage.IsCageEmpty())
            {
                availableCages.push_back(&cage);
            }
        }

        if (availableCages.empty())
        {
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
            cwl::WriteLine(_T("%cCurrently no visitors...\n"), PINK);
        }

        for (const auto& visitor : m_visitors)
        {
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
                if (s_cages.at(cageNumber).IsCageEmpty())
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

        for (auto& thread : s_cages)
        {
            thread.WaitForThreads();
        }
    }

    Zoo::Zoo() : m_canAddNewVisitorsEvent(wil::EventOptions::ManualReset)
    {
        //GameManager::Score = 0;

        m_newVisitorsTimer.reset(CreateWaitableTimer(nullptr, true, nullptr));
        THROW_LAST_ERROR_IF(!m_newVisitorsTimer.is_valid());

        m_addVisitorsThread.reset(CreateThread(nullptr, 0, AddVisitorsThread, this, 0, nullptr));

        s_isOpen = TRUE;

        m_canAddNewVisitorsEvent.SetEvent();
        ResetAddVisitorsEvent();

        m_zooTimerThread.reset(CreateThread(nullptr, 0, OpenZooTimerThread, this, 0, nullptr));
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

        THROW_LAST_ERROR_IF(
            !SetWaitableTimer(m_newVisitorsTimer.get(), &m_addVisitorsEventTime, 0, nullptr, nullptr, false)
        );
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
                zoo->m_visitors.emplace_back(wil::make_unique_failfast<Visitor>());
            }

            zoo->ResetAddVisitorsEvent();
        }
        while (true);
    }
}
