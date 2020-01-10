#include <ConsoleColors.h>
#include <tchar.h>
#include <cwl.h>
#include "Animal.h"

#include <time.h>



#pragma region Animal Functions

void AddAnimal(ZooAnimal* animal) {
    AnimalList* newListItem = static_cast<AnimalList*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AnimalList)));

    if (animal == NULL || newListItem == NULL) {
        cwl::WriteLine(_T("%cFailed to allocate memory for new list: %d\n"), RED, GetLastError());
        return;
    }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Flink;

    newListItem->LinkedList.Flink = temp;
    newListItem->LinkedList.Blink = animalListHead;
    temp->Blink = &newListItem->LinkedList;
    animalListHead->Flink = &newListItem->LinkedList;

    newListItem->ZooAnimal = *animal;

    LeaveCriticalSection(&AnimalListCrit);
}

void RemoveAnimal(ZooAnimal* animal) {
    if (IS_LIST_EMPTY(animalListHead)) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

    CRITICAL_SECTION animalRemovalCrit;

    if (!InitializeCriticalSectionAndSpinCount(&animalRemovalCrit, 4000)) {
        cwl::WriteLine(_T("Failed to initialize CRITICAL_SECTION: %d\n"), GetLastError());
        return;
    }

    EnterCriticalSection(&animalRemovalCrit);

    AnimalList* tempAnimal = CONTAINING_RECORD(animal, AnimalList, ZooAnimal);

    tempAnimal->LinkedList.Flink->Blink = tempAnimal->LinkedList.Blink;
    tempAnimal->LinkedList.Blink->Flink = tempAnimal->LinkedList.Flink;

    AddRandomName(animal->UniqueName);

    HeapFree(GetProcessHeap(), 0, tempAnimal);

    LeaveCriticalSection(&animalRemovalCrit);

    DeleteCriticalSection(&animalRemovalCrit);
}

#pragma endregion

#pragma region Animal Get Functions

void GetAllAnimals() {
    if (IS_LIST_EMPTY(animalListHead)) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        cwl::WriteLine(
            _T("[%c%s%r] %s the %s\n"),
            SKYBLUE,
            tempAnimal.CageName,
            tempAnimal.UniqueName,
            AnimalTypeToString(tempAnimal.AnimalType)
        );

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);
}

void GetAllAnimalsHealth() {
    if (IS_LIST_EMPTY(animalListHead)) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        cwl::WriteLine(
            _T("[%c%s%r] %s the %s "),
            SKYBLUE,
            tempAnimal.CageName,
            tempAnimal.UniqueName,
            AnimalTypeToString(tempAnimal.AnimalType)
        );

        if (tempAnimal.HealthLevel >= 6) {
            cwl::WriteLine(_T("(%c%d%r)\n"), LIME, tempAnimal.HealthLevel);
        } else if (tempAnimal.HealthLevel >= 3) {
            cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, tempAnimal.HealthLevel);
        } else {
            cwl::WriteLine(_T("(%c%d%r)\n"), PINK, tempAnimal.HealthLevel);
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);
}

void GetAllAnimalsInteractivity() {
    if (IS_LIST_EMPTY(animalListHead)) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        cwl::WriteLine(
            _T("[%c%s%r] %s the %s "),
            SKYBLUE,
            tempAnimal.CageName,
            tempAnimal.UniqueName,
            AnimalTypeToString(tempAnimal.AnimalType)
        );

        if (tempAnimal.InteractiveLevel >= 6) {
            cwl::WriteLine(_T("(%c%d%r)\n"), LIME, tempAnimal.InteractiveLevel);
        } else if (tempAnimal.InteractiveLevel >= 3) {
            cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, tempAnimal.InteractiveLevel);
        } else {
            cwl::WriteLine(_T("(%c%d%r)\n"), PINK, tempAnimal.InteractiveLevel);
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);
}

#pragma endregion

#pragma region Animal Change Functions

// Sets the health event after checking HeathLevel
void SetHealthEvent(ZooAnimal* animal) {
    if (animal->HealthLevel < 1) {
        cwl::WriteLine(
            _T("\n%c%s the %s is seriously ill and the Zoo Oversight Committee has relocated the animal\n\n"),
            PINK,
            animal->UniqueName,
            AnimalTypeToString(animal->AnimalType)
        );

        RemoveAnimal(animal);

        g_Score -= 3;
    } else if (animal->HealthLevel < 5) {
        cwl::WriteLine(_T("%s the %s is %csick\n"), animal->UniqueName, AnimalTypeToString(animal->AnimalType), PINK);
    }

    SetEvent(healthEvent);
}

// Increments HealthLevel by a number between FEED_LEVEL_MAX and FEED_LEVEL_MIN
void AddHealthLevel(ZooAnimal* animal) {
    const DWORD healthChange = rand() % (FEED_LEVEL_MAX - 1) + FEED_LEVEL_MIN;
    const DWORD newValue = animal->HealthLevel + healthChange;

    if (animal->HealthLevel >= 10) {
        cwl::WriteLine(
            _T("%c%s the %s is already full...\n"),
            LIME,
            animal->UniqueName,
            AnimalTypeToString(animal->AnimalType)
        );
        return;
    }

    if (newValue >= 10) {
        animal->HealthLevel = 10;
    } else {
        animal->HealthLevel = newValue;
    }

    animal->HealthLevelChange = TRUE;
    animal->InteractivityPrompted = TRUE;

    cwl::WriteLine(
        _T("%c%s the %s has been fed\n"),
        LIME,
        animal->UniqueName,
        AnimalTypeToString(animal->AnimalType)
    );
}

// Decrements HealthLevel by HUNGER_LEVEL
void RemoveHealthLevel(ZooAnimal* animal) {
    const DWORD newValue = animal->HealthLevel - HUNGER_LEVEL;

    if (newValue <= 0) {
        animal->HealthLevel = 0;
    } else {
        animal->HealthLevel = newValue;
    }

    animal->HealthLevelChange = TRUE;
    animal->InteractivityPrompted = FALSE;

    if (animal->HealthLevel < 5) {
        cwl::WriteLine(
            _T("%c%s the %s is hungry\n"),
            YELLOW,
            animal->UniqueName,
            AnimalTypeToString(animal->AnimalType)
        );
    }
}

// Increments InteractiveLevel by a number between FEED_LEVEL_MAX and FEED_LEVEL_MIN
void AddInteractiveLevel(ZooAnimal* animal) {
    const DWORD interactiveChange = rand() % (FEED_LEVEL_MAX - 1) + FEED_LEVEL_MIN;
    const DWORD newValue = animal->InteractiveLevel + interactiveChange;

    if (animal->InteractiveLevel >= 10) { return; }

    if (newValue >= 10) {
        animal->InteractiveLevel = 10;
    } else {
        animal->InteractiveLevel = newValue;
    }
}

// Decrements InteractiveLevel by HUNGER_LEVEL
void RemoveInteractiveLevel(ZooAnimal* animal) {
    const DWORD newValue = animal->InteractiveLevel - HUNGER_LEVEL;

    if (animal->InteractiveLevel <= 0) { return; }

    if (newValue <= 0) {
        animal->InteractiveLevel = 0;
    } else {
        animal->InteractiveLevel = newValue;
    }
}

#pragma endregion

#pragma region Cage Get Functions

BOOL IsCageEmpty(LPCTSTR cageName) {
    if (IS_LIST_EMPTY(animalListHead)) {
        return TRUE;
    }

    int count = 0;

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        if (_tcscmp(tempAnimal.CageName, cageName) == 0) {
            count++;
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);

    return count > 0 ? FALSE : TRUE;
}

DWORD GetCageTotalInteractiveLevel(LPCTSTR cageName) {
    DWORD total = 0;

    if (IS_LIST_EMPTY(animalListHead)) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return total;
    }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        if (_tcscmp(tempAnimal.CageName, cageName) == 0) {
            total += tempAnimal.InteractiveLevel;
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);

    return total;
}

DWORD GetCageAverageInteractiveLevel(LPCTSTR cageName) {
    DWORD total = 0;
    DWORD count = 0;

    if (IS_LIST_EMPTY(animalListHead)) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return total;
    }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        if (_tcscmp(tempAnimal.CageName, cageName) == 0) {
            total += tempAnimal.InteractiveLevel;
            count++;
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);

    return total / count;
}

// Gets a random cage and returns the cage pointer or returns NULL if the random cage is empty
Cage* GetRandomCage() {
    const int value = rand() % MAX_CAGES;
    int animalCount = 0;

    if (IS_LIST_EMPTY(animalListHead)) { return NULL; }

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        if (_tcscmp(cages[value]->Name, tempAnimal->CageName) == 0) {
            animalCount++;
        }

        temp = temp->Blink;
    }

    return animalCount > 0 ? cages[value] : NULL;
}

#pragma endregion

#pragma region Cage Functions

Cage* NewCage(LPCTSTR cageName) {
    if (healthEvent == NULL) {
        healthEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // Temporary, will be moved to NewCage

        if (healthEvent == NULL) {
            cwl::WriteLine(_T("%cFailed to create event: %d\n"), RED, GetLastError());
            return NULL;
        }
    }

    Cage* cage = static_cast<Cage*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Cage)));

    if (cage == 0) {
        cwl::WriteLine(_T("%cFailed to create cage\n"), RED);
        return NULL;
    }

    HANDLE feedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (feedEvent == NULL) {
        cwl::WriteLine(_T("%cFailed to create event: %d\n"), GetLastError());
        return NULL;
    }

    HANDLE feedEventTimer = CreateWaitableTimer(NULL, FALSE, NULL);

    if (feedEventTimer == NULL) {
        cwl::WriteLine(_T("%cFailed to create feed event timer: %d"), GetLastError());
        return NULL;
    }

    cage->Name = cageName;
    cage->FeedEvent = feedEvent;
    cage->FeedEventTimer = feedEventTimer;

    if (!ResetFeedTimer(feedEventTimer)) {
        return NULL;
    }

    cage->AnimalHealthThread = CreateThread(NULL, 0, &AnimalHealth, cage, 0, NULL);
    cage->AnimalInteractivityThread = CreateThread(NULL, 0, &AnimalInteractivity, NULL, 0, NULL);

    return cage;
}

BOOL ResetFeedTimer(HANDLE eventTimer) {
    if (SetWaitableTimer(eventTimer, &feedDueTime, 0, NULL, NULL, FALSE)) {
        return TRUE;
    }

    cwl::WriteLine(_T("Failed to set Feed Event Timer: %d\n"), GetLastError());

    return FALSE;
}

#pragma endregion

#pragma region Animal Thread Functions

DWORD WINAPI AnimalHealth(LPVOID lpParam) {
    Cage* cage = static_cast<Cage*>(lpParam);
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
    } while (TRUE);
}

DWORD WINAPI AnimalInteractivity(LPVOID lpParam) {
    lpParam = NULL;

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
    } while (TRUE);
}

DWORD WINAPI SignificantEventTimer(LPVOID) {

    srand((unsigned)time(NULL) * GetProcessId(GetCurrentProcess()));

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
            randomCage = GetRandomCage();
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
    } while (TRUE);
}

#pragma endregion
