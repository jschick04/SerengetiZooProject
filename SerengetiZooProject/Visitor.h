#pragma once

#include <Windows.h>
#include <wil/resource.h>

enum class VisitorStatus { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

class Visitor {
public:
    LPCTSTR UniqueName;
    LPCTSTR CageLocation;
    DWORD HappinessLevel;
    VisitorStatus Status;

    static wil::critical_section CriticalSection;

    explicit Visitor();

    void GetVisitorCount();
    void RemoveVisitor(LPCTSTR name);

private:
    wil::unique_handle m_visitorLoopThread;

    static DWORD WINAPI AddVisitorsThread(LPVOID lpParam); // Move to Zoo
    static DWORD WINAPI VisitorLoopThread(LPVOID lpParam);
};