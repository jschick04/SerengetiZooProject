#pragma once

#include <wil/resource.h>
#include <Windows.h>

enum class VisitorStatus { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

class Visitor {
public:
    LPCTSTR UniqueName;
    LPCTSTR CageLocation;
    DWORD HappinessLevel;
    VisitorStatus Status;

    static wil::critical_section CriticalSection;

    explicit Visitor();

private:
    wil::unique_handle m_movementTimer;
    wil::unique_handle m_visitorLoopThread;

    void ResetMovementTimer();

    static DWORD WINAPI VisitorLoop(LPVOID lpParam);
};
