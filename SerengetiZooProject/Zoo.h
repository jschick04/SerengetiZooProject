#pragma once

#include <vector>
#include <wil/resource.h>
#include "Cage.h"
#include "SignificantEvent.h"
#include "Visitor.h"

class Zoo {
public:
    static bool IsOpen;

    std::vector<wistd::unique_ptr<Cage>> Cages;
    std::vector<wistd::unique_ptr<Visitor>> Visitors;
    wistd::unique_ptr<SignificantEvent> Event;

    explicit Zoo(int numberOfCages);

    static void EndTurn();

    bool IsZooEmpty();

    void GetAllAnimals();
    void GetAllAnimalsHealth();
    void GetAllAnimalsInteractivity();

    Cage* GetRandomCage();

    void GetAllVisitors();

    void OpenZoo();

    void ShowCaseAnimals(int cageNumber);
private:
    wil::critical_section m_cs;

    static LARGE_INTEGER m_closedEventTime;
    wil::unique_event m_enterEvent;
    wil::unique_event m_newVisitorEvent;

    static void ResetClosedTimer();

    static DWORD WINAPI ClosedTimerThread(LPVOID);
};
