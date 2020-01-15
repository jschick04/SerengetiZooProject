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

    void GetAllVisitors() const;
    static int GetVisitorCount();

    void ShowCaseAnimals(int cageNumber);

    void WaitForThreads() const;

private:
    static std::vector<wistd::unique_ptr<Visitor>> m_visitors;

    wil::unique_handle m_addVisitorsThread;
    wil::unique_handle m_zooTimerThread;
    static LARGE_INTEGER m_closedEventTime;
    static wil::unique_event_failfast m_canAddNewVisitorsEvent;
    static LARGE_INTEGER m_addVisitorsEventTime;
    static wil::unique_handle m_newVisitorsTimer;

    static void RemoveVisitor(const wistd::unique_ptr<Visitor>& visitor);

    static void ResetAddVisitorsEvent();
    static void ResetOpenZooTimer();

    static DWORD WINAPI OpenZooTimerThread(LPVOID);
    static DWORD WINAPI AddVisitorsThread(LPVOID lpParam);
};
