#include "Cage.h"
#include <algorithm>
#include <ConsoleColors.h>
#include <cwl.h>
#include <string>
#include <tchar.h>
#include "Helpers.h"

wil::critical_section Cage::CriticalSection;

Cage::Cage(const int number) {
    Name = GetCageName(number);

    FeedEvent.create(wil::EventOptions::Signaled);
    THROW_LAST_ERROR_IF(!FeedEvent.is_valid());

    m_feedEventTimer.reset(CreateWaitableTimer(nullptr, false, nullptr));
    THROW_LAST_ERROR_IF_NULL(m_feedEventTimer);

    m_animalHealthThread.reset(CreateThread(nullptr, 0, AnimalHealth, nullptr, 0, nullptr));
    m_animalInteractivityThread.reset(CreateThread(nullptr, 0, AnimalInteractivity, nullptr, 0, nullptr));

    HealthEvent.create(wil::EventOptions::Signaled);
    THROW_LAST_ERROR_IF(!HealthEvent.is_valid());

    //if (!ResetFeedTimer(feedEventTimer)) {
    //    return NULL;
    //}
}

bool Cage::IsCageEmpty() const noexcept {
    auto guard = CriticalSection.lock();

    return Animals.empty();
}

DWORD Cage::GetAverageInteractiveLevel() const noexcept {
    DWORD total = 0;
    DWORD count = 0;

    auto guard = CriticalSection.lock();

    if (IsCageEmpty()) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return total;
    }

    for (const auto& animal : Animals) {
        count++;
        total += animal->InteractiveLevel;
    }

    return total / count;
}

DWORD Cage::GetTotalInteractiveLevel() const noexcept {
    DWORD total = 0;

    auto guard = CriticalSection.lock();

    if (IsCageEmpty()) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return total;
    }

    for (const auto& animal : Animals) {
        total += animal->InteractiveLevel;
    }

    return total;
}

// Adds an animal to the cage list
void Cage::AddAnimal(wistd::unique_ptr<Animal> animal) {
    auto guard = CriticalSection.lock();

    Animals.push_back(move(animal));
}

// Completely removes an animal from the cage and zoo
void Cage::RemoveAnimal(LPCTSTR uniqueName) {
    auto guard = CriticalSection.lock();

    Helpers::AddRandomName(uniqueName);

    Animals.erase(
        std::remove_if(
            Animals.begin(),
            Animals.end(),
            [&](wistd::unique_ptr<Animal>& cagedAnimal) { return cagedAnimal->UniqueName == uniqueName; }
        ),
        Animals.end()
    );
}

// Waits for Animal Health and Interactivity threads to close
void Cage::WaitForThreads() const noexcept {
    WaitForSingleObject(m_animalHealthThread.get(), INFINITE);
    WaitForSingleObject(m_animalInteractivityThread.get(), INFINITE);
}

// Converts a number to "Cage#" string
LPCTSTR Cage::GetCageName(const int number) {
    /*_tstringstream cageName;
    cageName << _T("Cage") << number;

    return cageName.str().c_str();*/
    switch (number) {
        case 1:
            return _T("Cage1");
        case 2:
            return _T("Cage2");
        case 3:
            return _T("Cage3");
        case 4:
            return _T("Cage4");
        case 5:
            return _T("Cage5");
        case 6:
            return _T("Cage6");
        case 7:
            return _T("Cage7");
        case 8:
            return _T("Cage8");
        case 9:
            return _T("Cage9");
        case 10:
            return _T("Cage10");
        case 11:
            return _T("Cage11");
        case 12:
            return _T("Cage12");
        default :
            return _T("Invalid Cage");
    }
}

DWORD WINAPI Cage::AnimalHealth(LPVOID) {
    /*Cage* cage = static_cast<Cage*>(lpParam);
    HANDLE events[3];

    srand((unsigned)time(NULL) * GetProcessId(GetCurrentProcess()));

    if (cage->FeedEvent == NULL) {
        cwl::WriteLine(_T("%cFeed Event is NULL\n"), RED);
        return 1;
    }

    events[0] = cage->FeedEvent;
    events[1] = cage->FeedEventTimer;
    events[2] = appClose;

    do {
        const DWORD waitResult = WaitForMultipleObjects(3, events, FALSE, INFINITE);

        if (waitResult == 2) {
            return 0;
        }

        if (IS_LIST_EMPTY(animalListHead)) { continue; }

        EnterCriticalSection(&AnimalListCrit);

        NodeEntry* temp = animalListHead->Blink;

        while (temp != animalListHead) {
            ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

            if (_tcscmp(tempAnimal->CageName, cage->Name) == 0) {
                if (waitResult == 0) {
                    AddHealthLevel(tempAnimal);
                } else if (waitResult == 1) {
                    RemoveHealthLevel(tempAnimal);
                }

                ResetFeedTimer(cage->FeedEventTimer);

                SetHealthEvent(tempAnimal);
            }

            temp = temp->Blink;
        };

        LeaveCriticalSection(&AnimalListCrit);
    } while (TRUE);*/

    return 0;
}

DWORD WINAPI Cage::AnimalInteractivity(LPVOID) {
    /*lpParam = NULL;

    HANDLE events[2];

    srand((unsigned)time(NULL) * GetProcessId(GetCurrentProcess()));

    events[0] = healthEvent;
    events[1] = appClose;

    do {
        if (WaitForMultipleObjects(2, events, FALSE, INFINITE) == 1) {
            return 0;
        }

        if (IS_LIST_EMPTY(animalListHead)) { continue; }

        EnterCriticalSection(&AnimalListCrit);

        NodeEntry* temp = animalListHead->Blink;

        while (temp != animalListHead) {
            ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

            if (tempAnimal->HealthLevelChange) {

                if (tempAnimal->InteractivityPrompted) {
                    AddInteractiveLevel(tempAnimal);
                } else {
                    RemoveInteractiveLevel(tempAnimal);
                }

                tempAnimal->HealthLevelChange = FALSE;
            }

            temp = temp->Blink;
        };

        LeaveCriticalSection(&AnimalListCrit);
    } while (TRUE);*/

    return 0;
}
