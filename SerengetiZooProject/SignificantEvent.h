#pragma once

#include <vector>
#include <wil/resource.h>
#include <Windows.h>

class Cage;

typedef struct EventParams {
    std::vector<Cage*> Cages;
    HANDLE Timer = nullptr;
} EventParams;

class SignificantEvent {
public:
    static LARGE_INTEGER DueTime;

    explicit SignificantEvent(std::vector<Cage*> cages);

    void WaitForThread() const noexcept;

private:
    wil::unique_handle m_significantEventThread;
    wil::unique_handle m_timer;

    static DWORD WINAPI SignificantEventTimer(LPVOID lpParam);
};
