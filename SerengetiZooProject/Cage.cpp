#include "Cage.h"
#include <algorithm>
#include <ConsoleColors.h>
#include <cwl.h>
#include <tchar.h>

Cage::Cage(const TCHAR* name) {
    Name = name;

    m_feedEvent.create(wil::EventOptions::Signaled);
    THROW_LAST_ERROR_IF(!m_feedEvent.is_valid());

    /*m_feedEventTimer.reset(CreateWaitableTimer(nullptr, false, nullptr));
    THROW_LAST_ERROR_IF_NULL(m_feedEventTimer);*/

    // TODO: Add Threads back
    //m_animalHealthThread.reset(CreateThread(nullptr, 0, &AnimalHealth, nullptr, 0, nullptr));
    //m_animalInteractivityThread.reset(CreateThread(nullptr, 0, &AnimalInteractivity, nullptr, 0, nullptr));

    // OLD CODE
    //if (healthEvent == NULL) {
    //    healthEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // Temporary, will be moved to NewCage

    //    if (healthEvent == NULL) {
    //        cwl::WriteLine(_T("%cFailed to create event: %d\n"), RED, GetLastError());
    //        return NULL;
    //    }
    //}

    //if (!ResetFeedTimer(feedEventTimer)) {
    //    return NULL;
    //}
}

bool Cage::IsCageEmpty() {
    auto guard = m_cs.lock();

    return Animals.empty();
}

DWORD Cage::GetAverageInteractiveLevel() {
    DWORD total = 0;
    DWORD count = 0;

    auto guard = m_cs.lock();

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

DWORD Cage::GetTotalInteractiveLevel() {
    DWORD total = 0;

    auto guard = m_cs.lock();

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
    auto guard = m_cs.lock();

    Animals.push_back(move(animal));
}

// Completely removes an animal from the cage and zoo
void Cage::RemoveAnimal(wistd::unique_ptr<Animal> animal) {
    /*auto guard = m_cs.lock();

    Animals.erase(
        std::remove_if(
            Animals.begin(),
            Animals.end(),
            [&](wistd::unique_ptr<Animal> cagedAnimal) { return cagedAnimal->UniqueName == animal->UniqueName; }
        ),
        Animals.end()
    );*/
}

void Cage::WaitForThreads() const noexcept {
    /*WaitForSingleObject(m_animalHealthThread.get(), INFINITE);
    WaitForSingleObject(m_animalInteractivityThread.get(), INFINITE);*/
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

DWORD WINAPI Cage::SignificantEventTimer(LPVOID) {
    /*srand((unsigned)time(NULL) * GetProcessId(GetCurrentProcess()));

    HANDLE events[2];

    events[0] = significantEventTimer;
    events[1] = appClose;

    do {
        if (WaitForMultipleObjects(2, events, FALSE, INFINITE) == 1) {
            return 0;
        }

        BOOL action;
        ZooAnimal* selectedAnimal = NULL;

        if (IS_LIST_EMPTY(animalListHead)) { continue; }

        EnterCriticalSection(&AnimalListCrit);

        Cage* randomCage;

        do {
            randomCage = GetRandomCageNumber();
        } while (randomCage == NULL);

        NodeEntry* temp = animalListHead->Blink;

        while (selectedAnimal == NULL) {
            if (temp == animalListHead) { temp = temp->Blink; }

            ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

            if (_tcscmp(tempAnimal->CageName, randomCage->Name) == 0) {
                action = rand() % 2;

                if (action) {
                    selectedAnimal = tempAnimal;
                    break;
                }
            }

            temp = temp->Blink;
        };

        action = rand() % 2;

        if (selectedAnimal != NULL) {
            if (action) {
                cwl::WriteLine(
                    _T("\n%c%s the %s has escaped!\n\n"),
                    YELLOW,
                    selectedAnimal->UniqueName,
                    AnimalTypeToString(selectedAnimal->AnimalType)
                );
                cwl::WriteLine(
                    _T("You have %clost%r %d points because all visitors left the zoo...\n"),
                    PINK,
                    GetVisitorCount(visitorListHead)
                );
                g_Score -= GetVisitorCount(visitorListHead);

                RemoveAnimal(selectedAnimal);
            } else {
                cwl::WriteLine(
                    _T("\n%c%s the %s has given birth to a baby %s!\n\n"),
                    LIME,
                    selectedAnimal->UniqueName,
                    AnimalTypeToString(selectedAnimal->AnimalType),
                    AnimalTypeToString(selectedAnimal->AnimalType)
                );
                cwl::WriteLine(
                    _T("All visitors have left for the day and you %cearned%r %d points...\n"),
                    LIME,
                    3 * GetVisitorCount(visitorListHead)
                );
                g_Score += 3 * (int)GetVisitorCount(visitorListHead);

                NewAnimal(
                    selectedAnimal->AnimalType,
                    GetRandomName(),
                    selectedAnimal->CageName
                );
            }
        }

        LeaveCriticalSection(&AnimalListCrit);

        if (!SetWaitableTimer(significantEventTimer, &seDueTime, 0, NULL, NULL, FALSE)) {
            cwl::WriteLine(_T("Failed to set Significant Event Timer: %d\n"), GetLastError());
        }

        EndTurnActions();
    } while (TRUE);*/

    return 0;
}
