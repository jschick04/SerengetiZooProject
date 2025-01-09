#pragma once

#include <functional>

namespace SerengetiZoo
{
    using Action = std::function<void()>;

    class SignificantEvent
    {
    public:
        explicit SignificantEvent();

    public:
        void AddListener(Action action);
        void RemoveListener(Action action);

        void Invoke() const;

        void ResetTimer() const noexcept;

        void WaitForThread() const noexcept;

    private:
        static DWORD WINAPI SignificantEventTimer(LPVOID lpParam);

    private:
        LARGE_INTEGER m_dueTime;

        std::vector<Action> m_callbacks;

        wil::unique_handle m_significantEventThread;
        wil::unique_handle m_timer;
    };
}
