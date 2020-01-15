#include "Zoo.h"

#include <ConsoleColors.h>
#include <cwl.h>
#include <random>
#include <tchar.h>
#include "GameManager.h"
#include "Helpers.h"
#include "Menu.h"

bool Zoo::IsOpen = false;
wil::critical_section Zoo::CriticalSection;

LARGE_INTEGER Zoo::m_closedEventTime;
wil::unique_handle Zoo::m_newVisitorsTimer;
wil::unique_event_failfast Zoo::m_canAddNewVisitorsEvent;
LARGE_INTEGER Zoo::m_addVisitorsEventTime;
std::vector<wistd::unique_ptr<Visitor>> Zoo::m_visitors;

Zoo::Zoo(const int numberOfCages) {
    GameManager::Score = 0;

    GameManager::OpenZoo.reset(CreateWaitableTimer(nullptr, false, nullptr));
    THROW_LAST_ERROR_IF(!GameManager::OpenZoo.is_valid());

    for (int i = 1; i <= numberOfCages; i++) {
        auto cage = wil::make_unique_failfast<Cage>(i);
        THROW_LAST_ERROR_IF_NULL(cage);

        Cages.push_back(move(cage));
    }

    auto cages = std::vector<Cage*>();
    for (auto const& cage : Cages) {
        cages.push_back(cage.get());
    }

    Event = wil::make_unique_failfast<SignificantEvent>(move(cages));
    THROW_LAST_ERROR_IF_NULL(Event);

    m_newVisitorsTimer.reset(CreateWaitableTimer(nullptr, true, nullptr));
    THROW_LAST_ERROR_IF(!m_newVisitorsTimer.is_valid());

    m_canAddNewVisitorsEvent.reset(CreateEvent(nullptr, true, false, nullptr));
    THROW_LAST_ERROR_IF(!m_canAddNewVisitorsEvent.is_valid());

    m_addVisitorsThread.reset(CreateThread(nullptr, 0, AddVisitorsThread, this, 0, nullptr));

    IsOpen = true;

    m_canAddNewVisitorsEvent.SetEvent();
    ResetAddVisitorsEvent();

    m_zooTimerThread.reset(CreateThread(nullptr, 0, OpenZooTimerThread, nullptr, 0, nullptr));
}

// Closes the zoo for the day and tells visitors to leave
void Zoo::EndTurn() {
    cwl::WriteLine(_T("\n%cZoo is closing for the rest of the day...\n"), PINK);

    m_canAddNewVisitorsEvent.ResetEvent();
    GameManager::CloseZoo.SetEvent();

    for (auto const& visitor : m_visitors) {
        visitor->WaitForThreads();
    }

    IsOpen = false;

    Menu::PrintScore();

    ResetOpenZooTimer();
}

// Checks all cages and returns if all cages are empty
bool Zoo::IsZooEmpty() {
    int count = 0;

    auto guard = Cage::CriticalSection.lock();

    for (auto const& cage : Cages) {
        if (cage->IsCageEmpty()) {
            count++;
        }
    }

    return count > 0;
}

// Prints all animals
void Zoo::GetAllAnimals() {
    auto guard = Cage::CriticalSection.lock();

    for (auto const& cage : Cages) {
        for (auto const& animal : cage->Animals) {
            cwl::WriteLine(
                _T("[%c%s%r] %s the %s\n"),
                SKYBLUE,
                animal->CurrentCage->Name,
                animal->UniqueName,
                Helpers::AnimalTypeToString(animal->AnimalType)
            );
        }
    }
}

// Prints all animals health values
void Zoo::GetAllAnimalsHealth() {
    auto cageGuard = Cage::CriticalSection.lock();

    for (auto const& cage : Cages) {
        if (cage->IsCageEmpty()) {
            cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage->Name);
            continue;
        }

        auto animalGuard = Animal::CriticalSection.lock();

        for (auto const& animal : cage->Animals) {
            cwl::WriteLine(
                _T("[%c%s%r] %s the %s "),
                SKYBLUE,
                animal->CurrentCage->Name,
                animal->UniqueName,
                Helpers::AnimalTypeToString(animal->AnimalType)
            );

            if (animal->HealthLevel >= 6) {
                cwl::WriteLine(_T("(%c%d%r)\n"), LIME, animal->HealthLevel);
            } else if (animal->HealthLevel >= 3) {
                cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, animal->HealthLevel);
            } else {
                cwl::WriteLine(_T("(%c%d%r)\n"), PINK, animal->HealthLevel);
            }
        }
    }
}

// Prints all animals interactivity values
void Zoo::GetAllAnimalsInteractivity() {
    auto cageGuard = Cage::CriticalSection.lock();

    for (auto const& cage : Cages) {
        if (cage->IsCageEmpty()) {
            cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage->Name);
            continue;
        }

        auto animalGuard = Animal::CriticalSection.lock();

        for (auto const& animal : cage->Animals) {
            cwl::WriteLine(
                _T("[%c%s%r] %s the %s "),
                SKYBLUE,
                animal->CurrentCage->Name,
                animal->UniqueName,
                Helpers::AnimalTypeToString(animal->AnimalType)
            );

            if (animal->InteractiveLevel >= 6) {
                cwl::WriteLine(_T("(%c%d%r)\n"), LIME, animal->InteractiveLevel);
            } else if (animal->InteractiveLevel >= 3) {
                cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, animal->InteractiveLevel);
            } else {
                cwl::WriteLine(_T("(%c%d%r)\n"), PINK, animal->InteractiveLevel);
            }
        }
    }
}

// Returns a random cage pointer
Cage* Zoo::GetRandomCage() {
    std::vector<Cage*> availableCages;

    auto guard = Cage::CriticalSection.lock();

    for (auto const& cage : Cages) {
        if (!cage->IsCageEmpty()) {
            availableCages.push_back(cage.get());
        }
    }

    return availableCages.empty() ? nullptr : availableCages.at(rand() % availableCages.size());
}

void Zoo::GetAllVisitors() {
    // TODO: Implement GetAllVisitors
    //DWORD WINAPI EnumVisitors(NodeEntry* VisitorListHead, BOOL PrintToConsole)
    //{
    //
    //    WaitForSingleObject(hVisitorEvent, INFINITE);
    //    EnterCriticalSection(&VisitorListCrit);
    //
    //    NodeEntry* EnumNode = static_cast<NodeEntry*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry)));
    //    if (EnumNode == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        //return NULL;
    //    }
    //    EnumNode = VisitorListHead->Flink;
    //    Visitor* eVisitor = static_cast<Visitor*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor)));
    //
    //    if (PrintToConsole == TRUE) { cwl::WriteLine(_T("[ Visitor ] [ Cage ] [ Happiness ] [ Status ]\n")); }
    //
    //    while (EnumNode->Flink != VisitorListHead->Flink)
    //    {
    //        LPCTSTR status = 0;
    //        eVisitor = CONTAINING_RECORD(EnumNode, Visitor, Links);
    //
    //        if (eVisitor->Status == 0)
    //        {
    //            status = _T("Happy");
    //        }
    //
    //        else if (eVisitor->Status == 1)
    //        {
    //            status = _T("Disappointed");
    //        }
    //
    //        else if (eVisitor->Status == 2)
    //        {
    //            status = _T("RefundDemanded");
    //        }
    //
    //        //perform the console print.
    //        if (PrintToConsole == TRUE && eVisitor->HappinessLevel <= 5) 
    //        { 
    //            cwl::WriteLine(_T("[ %c%s   -  %s  -  %d  -  %s  ]\n"),RED, eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, status); 
    //        }
    //        else if (PrintToConsole == TRUE && eVisitor->HappinessLevel >= 8)
    //        {
    //            cwl::WriteLine(_T("[ %c%s   -  %s  -  %d  -  %s  ]\n"),GREEN, eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, status);
    //        }
    //        else
    //        {
    //            cwl::WriteLine(_T("[ %c%s   -  %s  -  %d  -  %s  ]\n"),YELLOW, eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, status);
    //        }
    //
    //        EnumNode = EnumNode->Flink;
    //    }
    //
    //    LeaveCriticalSection(&VisitorListCrit);
    //    SetEvent(hVisitorEvent);
    //
    //
    //    return 0;
    //}
}

int Zoo::GetVisitorCount() {
    // TODO: Need to check all visitors not size of array
    return static_cast<int>(m_visitors.size());
}

void Zoo::ShowCaseAnimals(int cageNumber) {
    // TODO: Implement ShowCaseAnimals
    //auto cageName = static_cast<LPTSTR>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TCHAR) * 10));
    //const LPCTSTR prepend = _T("Cage");
    //if (cageName != 0) {
    //    StringCchPrintf(cageName, 10, _T("%s%d"), prepend, cagenum);
    //}

    //WaitForSingleObject(hVisitorEvent, INFINITE);
    //EnterCriticalSection(&VisitorListCrit);

    //NodeEntry* EnumNode = static_cast<NodeEntry*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry)));
    //if (EnumNode == NULL) {
    //    cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //    //return NULL;
    //}
    //EnumNode = VisitorListHead->Flink;
    //Visitor* eVisitor = static_cast<Visitor*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor)));

    //while (EnumNode->Flink != VisitorListHead->Flink) {
    //    eVisitor = CONTAINING_RECORD(EnumNode, Visitor, Links);
    //    if (eVisitor->CageLocation == cageName) {
    //        eVisitor->HappinessLevel = eVisitor->HappinessLevel + 1;
    //    }

    //    EnumNode = EnumNode->Flink;
    //}

    //LeaveCriticalSection(&VisitorListCrit);
    //SetEvent(hVisitorEvent);

    //return 0;
}

void Zoo::WaitForThreads() const {
    WaitForSingleObject(m_addVisitorsThread.get(), INFINITE);
    WaitForSingleObject(m_zooTimerThread.get(), INFINITE);

    Event->WaitForThread();

    for (auto& thread : Cages) {
        thread->WaitForThreads();
    }
}

void Zoo::RemoveVisitor(LPCTSTR uniqueName) {
    auto guard = Visitor::CriticalSection.lock();

    // TODO: Finish RemoveVisitor
    //{
    //    WaitForSingleObject(hVisitorEvent, INFINITE);
    //    EnterCriticalSection(&VisitorListCrit);
    //
    //    NodeEntry* RemovedNode = static_cast<NodeEntry*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry)));
    //    if (RemovedNode == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    NodeEntry* TempNodePrev = static_cast<NodeEntry*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry)));
    //    if (TempNodePrev == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    NodeEntry* TempNodeNext = static_cast<NodeEntry*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry)));;
    //    if (TempNodeNext == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    RemovedNode = VisitorListHead->Flink;
    //    Visitor* RemovedVisitor = static_cast<Visitor*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor)));
    //    if (RemovedVisitor == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    Visitor* PreviousVisitor = static_cast<Visitor*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor)));
    //    if (PreviousVisitor == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    Visitor* NextVisitor = static_cast<Visitor*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor)));
    //    if (NextVisitor == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //
    //    //loop through all to find a matching uniquename.
    //    while (RemovedVisitor->UniqueName != Name)
    //    {
    //        RemovedVisitor = CONTAINING_RECORD(RemovedNode, Visitor, Links);
    //        RemovedNode = RemovedNode->Flink;
    //    }
    //    //when a unique name is found we need to update the flink of the previous node and the blink of the next node.
    //    TempNodePrev = RemovedVisitor->Links.Blink;
    //    PreviousVisitor = CONTAINING_RECORD(TempNodePrev, Visitor, Links);
    //    TempNodeNext = RemovedVisitor->Links.Flink;
    //    NextVisitor = CONTAINING_RECORD(TempNodeNext, Visitor, Links);
    //    PreviousVisitor->Links.Flink = TempNodeNext;
    //    NextVisitor->Links.Blink = TempNodePrev;
    //
    //    //cleanup
    //    HeapFree(GetProcessHeap(), 0, RemovedVisitor);
    //
    //    LeaveCriticalSection(&VisitorListCrit);
    //    SetEvent(hVisitorEvent);
    //
    //    return 0;
    //}
}

void Zoo::ResetAddVisitorsEvent() {
    std::random_device generator;
    const std::uniform_int_distribution<int> range(5, 30);

    m_addVisitorsEventTime.QuadPart = -(range(generator) * TIMER_SECONDS);

    THROW_LAST_ERROR_IF(
        !SetWaitableTimer(m_newVisitorsTimer.get(), &m_addVisitorsEventTime, 0, nullptr, nullptr, false)
    );
}

// Resets the timer that triggers the Zoo opening
void Zoo::ResetOpenZooTimer() {
    m_closedEventTime.QuadPart = -(30 * TIMER_SECONDS);
    THROW_LAST_ERROR_IF(!SetWaitableTimer(GameManager::OpenZoo.get(), &m_closedEventTime, 0, nullptr, nullptr, 0));
}

DWORD WINAPI Zoo::OpenZooTimerThread(LPVOID) {
    HANDLE events[2];
    events[0] = GameManager::OpenZoo.get();
    events[1] = GameManager::AppClose.get();

    do {
        if (WaitForMultipleObjects(_countof(events), events, false, INFINITE) == 0) {
            GameManager::CloseZoo.ResetEvent();

            IsOpen = true;

            cwl::WriteLine(_T("\n%c------------------------------------\n"), RED);
            cwl::WriteLine(_T("%cThe Zoo is now open.\n"), RED);
            cwl::WriteLine(_T("%c------------------------------------\n\n"), RED);

            ResetAddVisitorsEvent();
            m_canAddNewVisitorsEvent.SetEvent();
        } else {
            return 0;
        }
    } while (TRUE);
}

DWORD WINAPI Zoo::AddVisitorsThread(LPVOID lpParam) {
    const auto zoo = static_cast<Zoo*>(lpParam);

    HANDLE exitEvents[2];

    exitEvents[0] = m_canAddNewVisitorsEvent.get();
    exitEvents[1] = GameManager::AppClose.get();

    HANDLE events[3];

    events[0] = m_newVisitorsTimer.get();
    events[1] = GameManager::CloseZoo.get();
    events[2] = GameManager::AppClose.get();

    do {
        const auto waitForZooOpen = WaitForMultipleObjects(_countof(exitEvents), events, false, INFINITE);

        if (waitForZooOpen == 1) { return 0; }

        const auto wait = WaitForMultipleObjects(_countof(events), events, false, INFINITE);

        if (wait == 1) { continue; }

        if (wait == 2) { return 0; }

        auto guard = Cage::CriticalSection.lock();

        std::random_device generator;
        const std::uniform_int_distribution<int> range(1, 4);

        const int visitorsToAdd = range(generator);
        std::vector<Cage*> cages;

        for (auto const& cage : zoo->Cages) {
            cages.push_back(cage.get());
        }

        for (int i = 0; i != visitorsToAdd; ++i) {
            auto visitor = wil::make_unique_nothrow<Visitor>(cages);
            RETURN_IF_NULL_ALLOC(visitor);

            m_visitors.push_back(move(visitor));
        }

        ResetAddVisitorsEvent();
    } while (true);
}
