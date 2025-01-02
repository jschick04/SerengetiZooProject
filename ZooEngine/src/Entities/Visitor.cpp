#include "zepch.h"

#include "Visitor.h"

#include "Game.h"
#include "Zoo.h"

namespace SerengetiZoo
{
    Visitor::Visitor()
    {
        m_name = Helpers::GetRandomName();
        m_location = _T("Entry");
        m_happiness = 8;
        m_status = VisitorStatus::Happy;

        m_currentCageNumber = -1;

        m_movementTimer.reset(CreateWaitableTimer(nullptr, false, nullptr));
        THROW_LAST_ERROR_IF(!m_movementTimer.is_valid());

        m_visitorLoopThread.reset(CreateThread(nullptr, 0, VisitorLoop, this, 0, nullptr));

        ResetMovementTimer();

        cwl::WriteLine(_T("%c%s has entered the Zoo\n"), LIME, m_name);
    }

    void Visitor::WaitForThreads() const noexcept
    {
        WaitForSingleObject(m_visitorLoopThread.get(), INFINITE);
    }

    void Visitor::AddHappiness()
    {
        if (m_happiness < 10)
        {
            m_happiness++;
        }
    }

    // Updates global score based on happiness level
    void Visitor::CalculateScore() noexcept
    {
        if (m_happiness < 5)
        {
            m_status = VisitorStatus::RefundDemanded;

            cwl::WriteLine(_T("%c%s is %s\n"), PINK, m_name, ToString(m_status));
            //GameManager::Score -= 3;
        }
        else if (m_happiness <= 7)
        {
            m_status = VisitorStatus::LeavingAngry;

            cwl::WriteLine(_T("%c%s is %s\n"), YELLOW, m_name, ToString(m_status));
            //GameManager::Score -= 1;
        }
        else
        {
            m_status = VisitorStatus::LeavingHappy;

            cwl::WriteLine(_T("%c%s is %s\n"), LIME, m_name, ToString(m_status));
            //GameManager::Score += 1;
        }
    }

    // Resets the movement timer to a random time
    void Visitor::ResetMovementTimer()
    {
        std::random_device generator;
        std::uniform_int_distribution range(1, 3);

        m_movementTime.QuadPart = -((range(generator) * 10) * TimerSeconds);

        THROW_LAST_ERROR_IF(!SetWaitableTimer(m_movementTimer.get(), &m_movementTime, 0, nullptr, nullptr, false));
    }

    // Updates status based on the happiness level
    void Visitor::UpdateStatus() noexcept
    {
        if (m_happiness < 5)
        {
            m_status = VisitorStatus::RefundDemanded;
        }
        else if (m_happiness <= 7)
        {
            m_status = VisitorStatus::Disappointed;
        }
        else
        {
            m_status = VisitorStatus::Happy;
        }
    }

    DWORD WINAPI Visitor::VisitorLoop(const LPVOID lpParam)
    {
        const auto visitor = static_cast<Visitor*>(lpParam);

        HANDLE events[3];

        events[0] = visitor->m_movementTimer.get();
        events[1] = Zoo::Close.get();
        events[2] = Game::EndGame.get();

        do
        {
            const auto wait = WaitForMultipleObjects(_countof(events), events, false, INFINITE);

            if (wait == 1 || wait == 2)
            {
                auto guard = visitor->m_cs.lock();

                visitor->CalculateScore();

                return 0;
            }

            auto guard = visitor->m_cs.lock();

            visitor->m_currentCageNumber++;

            if (visitor->m_currentCageNumber > GameSettings::MaxCages - 1 ||
                visitor->m_status == VisitorStatus::RefundDemanded)
            {
                visitor->CalculateScore();

                return 0;
            }

            auto& currentCage = Zoo::GetCages().at(visitor->m_currentCageNumber);

            if (currentCage.IsCageEmpty())
            {
                cwl::WriteLine(_T("%c%s lost 2 happiness points due to an empty cage\n"), PINK, visitor->m_name);
                visitor->m_happiness -= 2;

                visitor->UpdateStatus();

                visitor->ResetMovementTimer();

                continue;
            }

            cwl::WriteLine(_T("%s is interacting with the %s cage\n"), visitor->m_name, currentCage.GetType());

            const DWORD averageInteractivityLevel = currentCage.GetAverageInteractiveLevel();

            if (averageInteractivityLevel <= 4)
            {
                if (visitor->m_happiness > 0)
                {
                    visitor->m_happiness -= 1;

                    cwl::WriteLine(
                        _T("%s is %cDisappointed%r with the %s cage!\n"),
                        visitor->m_name,
                        RED,
                        currentCage.GetType());
                }
            }
            else
            {
                if (visitor->m_happiness > 5 && visitor->m_happiness < 10)
                {
                    visitor->m_happiness += 1;
                }
            }

            visitor->UpdateStatus();

            visitor->ResetMovementTimer();
        }
        while (true);
    }
}
