#include "SerengetiZooProject.h"
#include "Animals.h"
#include <WriteLine.h>
#include <tchar.h>
#include <ConsoleColors.h>

HANDLE healthEvent;

LPTSTR AnimalTypeToString(enum AnimalType animalType) {
    switch (animalType) {
        case Antelopes :
            return _T("Antelopes");
        case Cheetahs :
            return _T("Cheetahs");
        case Giraffes :
            return _T("Giraffes");
        case Hyaena :
            return _T("Hyaena");
        case Hippos :
            return _T("Hippos");
        case Monkeys :
            return _T("Monkeys");
        case Mongoose :
            return _T("Mongoose");
        case Tigers :
            return _T("Tigers");
        case WildeBeast :
            return _T("WildeBeast");
        case Zebras :
            return _T("Zebras");
        default :
            return _T("");
    }
}

void SetHealthEvent(ZooAnimal* animal) {
    if (healthEvent == NULL) {
        healthEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // Temporary, will be moved to NewCage

        if (healthEvent == NULL) {
            ConsoleWriteLine(_T("%cFailed to create event: %d"), RED, GetLastError());
            return;
        }
    }

    if (animal->HealthLevel <= 0) {
        ConsoleWriteLine(
            _T("%c%s the %s is seriously ill and the Zoo Oversight Committee has relocated the animal\n"),
            YELLOW,
            animal->UniqueName,
            AnimalTypeToString(animal->AnimalType)
        );

        RemoveAnimal(animal);

        g_Score -= 3;
    } else if (animal->HealthLevel < 5) {
        ConsoleWriteLine(_T("%s the %s is %csick\n"), animal->UniqueName, AnimalTypeToString(animal->AnimalType), PINK);
    }

    SetEvent(healthEvent);
}

#pragma region Animal Functions

ZooAnimal* NewAnimal(enum AnimalType animalType, LPTSTR uniqueName, LPTSTR cageName, DWORD interactiveLevel) {
    ZooAnimal* newAnimal = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ZooAnimal));

    if (newAnimal == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory for new animal: %d\n"), RED, GetLastError());
        return NULL;
    }

    newAnimal->AnimalType = animalType;
    newAnimal->UniqueName = uniqueName;
    newAnimal->CageName = cageName;
    newAnimal->InteractiveLevel = interactiveLevel;

    newAnimal->HealthLevel = 5;
    newAnimal->HealthLevelChange = FALSE;

    newAnimal->InteractivityPrompted = FALSE;

    newAnimal->FeedTimer = 10;

    AddAnimal(newAnimal);

    return newAnimal;
}

void AddAnimal(ZooAnimal* animal) {
    AnimalList* newListItem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AnimalList));

    if (animal == NULL || newListItem == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory for new list: %d\n"), RED, GetLastError());
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
    if (IS_LIST_EMPTY(animalListHead)) { return; }

    CRITICAL_SECTION animalRemovalCrit;

    if (!InitializeCriticalSectionAndSpinCount(&animalRemovalCrit, 4000)) {
        ConsoleWriteLine(_T("Failed to initialize CRITICAL_SECTION: %d"), GetLastError());
        return;
    }

    EnterCriticalSection(&animalRemovalCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const AnimalList* tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList);

        if (memcmp(animal, &tempAnimal->ZooAnimal, sizeof(animal)) == 0) {
            NodeEntry* node = temp;
            NodeEntry* flink = temp->Flink;
            temp = node->Blink;
            flink->Blink = temp;
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&animalRemovalCrit);

    DeleteCriticalSection(&animalRemovalCrit);
}

#pragma endregion

#pragma region Animal Get Functions

void GetAllAnimals() {
    if (IS_LIST_EMPTY(animalListHead)) { return; }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        ConsoleWriteLine(
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
    if (IS_LIST_EMPTY(animalListHead)) { return; }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        ConsoleWriteLine(
            _T("[%c%s%r] %s the %s "),
            SKYBLUE,
            tempAnimal.CageName,
            tempAnimal.UniqueName,
            AnimalTypeToString(tempAnimal.AnimalType)
        );

        if (tempAnimal.HealthLevel >= 6) {
            ConsoleWriteLine(_T("(%c%d%r)\n"), LIME, tempAnimal.HealthLevel);
        } else if (tempAnimal.HealthLevel >= 3) {
            ConsoleWriteLine(_T("(%c%d%r)\n"), YELLOW, tempAnimal.HealthLevel);
        } else {
            ConsoleWriteLine(_T("(%c%d%r)\n"), PINK, tempAnimal.HealthLevel);
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);
}

#pragma endregion

#pragma region Animal Change Functions

void DecreaseAnimalFedTimer() {
    if (IS_LIST_EMPTY(animalListHead)) { return; }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        if (tempAnimal->FeedTimer <= 0) {
            tempAnimal->HealthLevel--;
            tempAnimal->FeedTimer = 10;

            ConsoleWriteLine(
                _T("%c%s the %s is hungry\n"),
                YELLOW,
                tempAnimal->UniqueName,
                AnimalTypeToString(tempAnimal->AnimalType)
            );

            SetHealthEvent(tempAnimal);
        } else {
            tempAnimal->FeedTimer--;
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);
}

#pragma endregion

#pragma region Cage Get Functions

DWORD GetCageTotalInteractiveLevel(LPTSTR cageName) {
    DWORD total = 0;

    if (IS_LIST_EMPTY(animalListHead)) { return total; }

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

DWORD GetCageAverageInteractiveLevel(LPTSTR cageName) {
    DWORD total = 0;
    DWORD count = 0;

    if (IS_LIST_EMPTY(animalListHead)) { return total; }

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

#pragma endregion

#pragma region Cage Functions

Cage* NewCage(LPTSTR cageName) {
    Cage* cage = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Cage));

    if (cage == 0) {
        ConsoleWriteLine(_T("%cFailed to create cage\n"), RED);
        return NULL;
    }

    HANDLE feedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (feedEvent == NULL) {
        ConsoleWriteLine(_T("%cFailed to create event: %d\n"), GetLastError());
        return NULL;
    }

    cage->Name = cageName;
    cage->FeedEvent = feedEvent;
    cage->AnimalHealthThread = CreateThread(NULL, 0, &AnimalHealth, cage, 0, NULL);

    return cage;
}

#pragma endregion

#pragma region Animal Thread Functions

DWORD WINAPI AnimalHealth(LPVOID lpParam) {
    Cage* cage = lpParam;

    if (cage->FeedEvent == NULL) {
        ConsoleWriteLine(_T("%cFeed Event is NULL\n"), RED);
        return 1;
    }

    do {
        HANDLE events[] = { cage->FeedEvent, appClose };

        if (WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE) == 1) {
            return 0;
        }

        if (IS_LIST_EMPTY(animalListHead)) { continue; }

        EnterCriticalSection(&AnimalListCrit);

        NodeEntry* temp = animalListHead->Blink;

        while (temp != animalListHead) {
            ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

            if (_tcscmp(tempAnimal->CageName, cage->Name) == 0) {
                tempAnimal->HealthLevel++;
                tempAnimal->HealthLevelChange = TRUE;

                ConsoleWriteLine(
                    _T("%c%s the %s has been fed\n"),
                    LIME,
                    tempAnimal->UniqueName,
                    AnimalTypeToString(tempAnimal->AnimalType)
                );

                SetHealthEvent(tempAnimal);
            }

            temp = temp->Blink;
        };

        LeaveCriticalSection(&AnimalListCrit);
    } while (TRUE);
}

DWORD WINAPI AnimalInteractivity(LPVOID lpParam) {
    // TODO: Implement SignificantEventTimer

    // Waits for health event
    // Check for Animal->HealthLevelChange == TRUE
    // IF TRUE Animal->InteractiveLevel++ ELSE --
    return 0;
}

#pragma endregion
