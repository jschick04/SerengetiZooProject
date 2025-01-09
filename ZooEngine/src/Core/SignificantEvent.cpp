#include "zepch.h"

#include "SignificantEvent.h"

#include "Game.h"

namespace SerengetiZoo
{
    SignificantEvent::SignificantEvent()
    {
        m_callbacks.reserve(10);

        m_dueTime.QuadPart = -((GameSettings::SignificantEventMinutes * 60) * TimerSeconds);

        m_timer.reset(CreateWaitableTimer(nullptr, false, nullptr));
        THROW_LAST_ERROR_IF(!m_timer.is_valid());

        m_significantEventThread.reset(CreateThread(nullptr, 0, SignificantEventTimer, this, 0, nullptr));

        ResetTimer();
    }

    void SignificantEvent::AddListener(Action action)
    {
        const auto iterator = std::ranges::find_if(m_callbacks, [&action](const Action& callback)
        {
            return callback.target<void()>() == action.target<void()>();
        });

        if (iterator != m_callbacks.end()) { return; }

        m_callbacks.push_back(action);
    }

    void SignificantEvent::RemoveListener(Action action)
    {
        const auto iterator = std::ranges::find_if(m_callbacks, [&action](const Action& callback)
        {
            return callback.target<void()>() == action.target<void()>();
        });

        if (iterator == m_callbacks.end()) { return; }

        m_callbacks.erase(iterator, m_callbacks.end());
    }

    void SignificantEvent::Invoke() const
    {
        for (const auto& callback : m_callbacks)
        {
            callback();
        }
    }

    void SignificantEvent::ResetTimer() const noexcept
    {
        THROW_LAST_ERROR_IF(!SetWaitableTimer(m_timer.get(), &m_dueTime, 0, nullptr, nullptr, false));
    }

    void SignificantEvent::WaitForThread() const noexcept
    {
        WaitForSingleObject(m_significantEventThread.get(), INFINITE);
    }

    DWORD WINAPI SignificantEvent::SignificantEventTimer(const LPVOID lpParam)
    {
        const auto params = static_cast<SignificantEvent*>(lpParam);

        HANDLE events[2] { };

        events[0] = params->m_timer.get();
        events[1] = Game::EndGame.get();

        do
        {
            if (WaitForMultipleObjects(_countof(events), events, false, INFINITE) == 1)
            {
                return 0;
            }

            params->Invoke();

            Zoo::Close.SetEvent();
        }
        while (true);
    }
}
