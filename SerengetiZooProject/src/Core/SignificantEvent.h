#pragma once

#include <functional>

namespace SerengetiZoo
{
    class SignificantEvent
    {
    public:
        explicit SignificantEvent();

    public:
        void AddListener(void* action);
        void RemoveListener(void* action);

        void Invoke() const;

        void WaitForThread() const noexcept;

    private:
        static DWORD WINAPI SignificantEventTimer(LPVOID lpParam);

    private:
        LARGE_INTEGER m_dueTime;

        std::vector<void*> m_callbacks;

        wil::unique_handle m_significantEventThread;
        wil::unique_handle m_timer;
    };
}
