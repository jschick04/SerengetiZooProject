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
wil::unique_event_failfast Zoo::m_canAddNewVisitorsEvent(wil::EventOptions::ManualReset);
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

    m_addVisitorsThread.reset(CreateThread(nullptr, 0, AddVisitorsThread, this, 0, nullptr));

    IsOpen = true;

    m_canAddNewVisitorsEvent.SetEvent();
    ResetAddVisitorsEvent();

    m_zooTimerThread.reset(CreateThread(nullptr, 0, OpenZooTimerThread, nullptr, 0, nullptr));
}

// Closes the zoo for the day and tells visitors to leave
void Zoo::EndTurn() {
    cwl::WriteLine(_T("\n%cZoo is closing for the rest of the day...\n\n"), PINK);

    m_canAddNewVisitorsEvent.ResetEvent();
    GameManager::CloseZoo.SetEvent();

    auto end = m_visitors.end();

    for (auto i = m_visitors.size(); i-- > 0;) {

        m_visitors[i]->WaitForThreads();
        RemoveVisitor(m_visitors[i]->UniqueName);
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

// Prints the visitors currently in the zoo with their location and values
void Zoo::GetAllVisitors() const {
    if (m_visitors.empty()) {
        cwl::WriteLine(_T("%cCurrently no visitors...\n"), PINK);
    }

    for (auto const& visitor : m_visitors) {
        cwl::WriteLine(
            _T("[%c%s%r] %s is %s "),
            SKYBLUE,
            visitor->CageLocation,
            visitor->UniqueName,
            Helpers::VisitorStatusToString(visitor->Status)
        );

        if (visitor->HappinessLevel < 5) {
            cwl::WriteLine(_T("(%c%d)\n"), PINK, visitor->HappinessLevel);
        } else if (visitor->HappinessLevel <= 7) {
            cwl::WriteLine(_T("(%c%d)\n"), YELLOW, visitor->HappinessLevel);
        } else {
            cwl::WriteLine(_T("(%c%d)\n"), LIME, visitor->HappinessLevel);

        }
    }
}

int Zoo::GetVisitorCount() {
    return static_cast<int>(m_visitors.size());
}

// Increase happiness level for anyone in the specified cage
void Zoo::ShowCaseAnimals(int cageNumber) {
    auto guard = CriticalSection.lock();

    for (auto const& visitor : m_visitors) {
        if (visitor->CageLocation == Cages.at(cageNumber)->Name) {
            if (Cages.at(cageNumber)->IsCageEmpty()) {
                continue;
            }

            visitor->HappinessLevel++;
        }
    }
}

void Zoo::WaitForThreads() const {
    WaitForSingleObject(m_addVisitorsThread.get(), INFINITE);
    WaitForSingleObject(m_zooTimerThread.get(), INFINITE);

    Event->WaitForThread();

    for (auto& thread : Cages) {
        thread->WaitForThreads();
    }
}

// Removes a visitor from the visitor list and returns the unique name
void Zoo::RemoveVisitor(LPCTSTR name) {
    auto guard = CriticalSection.lock();

    Helpers::AddRandomName(name);

    m_visitors.erase(
        std::remove_if(
            m_visitors.begin(),
            m_visitors.end(),
            [&](wistd::unique_ptr<Visitor>& visitor) { return visitor->UniqueName == name; }
        ),
        m_visitors.end()
    );
}

// Resets the timer that adds users in the add visitors thread
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
    THROW_LAST_ERROR_IF(!SetWaitableTimer(GameManager::OpenZoo.get(), &m_closedEventTime, 0, nullptr, nullptr, false));
}

DWORD WINAPI Zoo::OpenZooTimerThread(LPVOID) {
    HANDLE events[2];
    events[0] = GameManager::OpenZoo.get();
    events[1] = GameManager::AppClose.get();

    do {
        if (WaitForMultipleObjects(_countof(events), events, false, INFINITE) == 0) {
            GameManager::CloseZoo.ResetEvent();

            IsOpen = true;

            cwl::WriteLine(_T("\n%c------------------------------------\n"), LIME);
            cwl::WriteLine(_T("%cThe Zoo is now open.\n"), LIME);
            cwl::WriteLine(_T("%c------------------------------------\n\n"), LIME);

            m_canAddNewVisitorsEvent.SetEvent();
            ResetAddVisitorsEvent();
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
        const auto waitForZooOpen = WaitForMultipleObjects(_countof(exitEvents), exitEvents, false, INFINITE);

        if (waitForZooOpen == 1) { return 0; }

        const auto wait = WaitForMultipleObjects(_countof(events), events, false, INFINITE);

        if (wait == 1) {
            continue;
        }

        if (wait == 2) {
            return 0;
        }

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
