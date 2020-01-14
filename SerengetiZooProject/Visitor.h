#pragma once

#include <wil/resource.h>
#include <Windows.h>
#include <vector>

class Cage;

enum class VisitorStatus { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

class Visitor {
public:
    LPCTSTR UniqueName;
    LPCTSTR CageLocation;
    DWORD HappinessLevel;
    VisitorStatus Status;

    static wil::critical_section CriticalSection;

    explicit Visitor(std::vector<Cage*> cages);

    void WaitForThreads() const noexcept;

private:
    LARGE_INTEGER m_movementTime;
    wil::unique_handle m_movementTimer;
    wil::unique_handle m_visitorLoopThread;

    std::vector<Cage*> m_cages;
    int m_currentCageNumber;

    static void CalculateScore(Visitor* visitor) noexcept;
    void ResetMovementTimer();
    static void UpdateStatus(Visitor* visitor) noexcept;

    static DWORD WINAPI VisitorLoop(LPVOID lpParam);
};
