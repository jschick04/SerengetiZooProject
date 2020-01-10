#pragma once

#include <Windows.h>

class SignificantEvent {
public:
    LARGE_INTEGER seDueTime;
    LARGE_INTEGER feedDueTime;

    SignificantEvent(LARGE_INTEGER EventTimer);
private:
    HANDLE m_significantEventThread;
    HANDLE m_significantEventTimer;

    DWORD WINAPI SignificantEventTimer(LPVOID);
};
