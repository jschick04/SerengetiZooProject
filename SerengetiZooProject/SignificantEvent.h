#pragma once

#include <Windows.h>
#include "Cage.h"

typedef struct EventParams {
    std::vector<Cage*> Cages;
    HANDLE Timer = nullptr;
} EventParams;

class SignificantEvent {
public:
    explicit SignificantEvent(std::vector<Cage*> cages);

    void WaitForThread() const noexcept;
private:
    LARGE_INTEGER m_dueTime;
    LARGE_INTEGER m_feedDueTime;

    wil::unique_handle m_significantEventThread;
    wil::unique_handle m_timer;

    static DWORD WINAPI SignificantEventTimer(LPVOID lpParam);
};
