#pragma once

#include <vector>
#include <wil/resource.h>
#include <Windows.h>
#include "Cage.h"
#include "SignificantEvent.h"
#include "Visitor.h"

class Zoo {
public:
    static bool IsOpen;
    static wil::critical_section CriticalSection;

    std::vector<wistd::unique_ptr<Cage>> Cages;
    wistd::unique_ptr<SignificantEvent> Event;

    explicit Zoo(int numberOfCages);

    static void EndTurn();

    bool IsZooEmpty();

    void GetAllAnimals();
    void GetAllAnimalsHealth();
    void GetAllAnimalsInteractivity();

    Cage* GetRandomCage();

    void GetAllVisitors();
    static int GetVisitorCount();

    void OpenZoo();

    void ShowCaseAnimals(int cageNumber);

private:
    static std::vector<wistd::unique_ptr<Visitor>> m_visitors;

    static LARGE_INTEGER m_closedEventTime;
    wil::unique_event m_enterEvent;
    wil::unique_event m_newVisitorEvent;

    void RemoveVisitor(LPCTSTR name);

    static void ResetClosedTimer();

    static DWORD WINAPI ClosedTimerThread(LPVOID);
    static DWORD WINAPI AddVisitorsThread(LPVOID lpParam);
};
