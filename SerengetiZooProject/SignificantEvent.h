#pragma once

#include <wil/resource.h>
#include <Windows.h>

class SignificantEvent {
public:
    explicit SignificantEvent();

    void WaitForThread() const noexcept;
private:
    LARGE_INTEGER m_dueTime;
    LARGE_INTEGER m_feedDueTime;

    wil::unique_handle m_significantEventThread;
    wil::unique_handle m_significantEventTimer;

    static DWORD WINAPI SignificantEventTimer(LPVOID);
};
