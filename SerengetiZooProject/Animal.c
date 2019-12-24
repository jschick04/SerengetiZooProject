#include <ConsoleColors.h>
#include <tchar.h>
#include <time.h>
#include <WriteLine.h>
#include "Animals.h"
#include "SerengetiZooProject.h"

#pragma region Function Declarations

void SetHealthEvent(ZooAnimal* animal);
DWORD WINAPI AnimalHealth(LPVOID lpParam);
DWORD WINAPI AnimalInteractivity(LPVOID lpParam);

#pragma endregion

HANDLE healthEvent;

LPTSTR AnimalTypeToString(enum AnimalType animalType) {
    switch (animalType) {
        case Antelope :
            return _T("Antelope");
        case Cheetah :
            return _T("Cheetah");
        case Giraffe :
            return _T("Giraffe");
        case Hyaena :
            return _T("Hyaena");
        case Hippo :
            return _T("Hippo");
        case Monkey :
            return _T("Monkey");
        case Mongoose :
            return _T("Mongoose");
        case Tiger :
            return _T("Tiger");
        case WildeBeast :
            return _T("WildeBeast");
        case Zebra :
            return _T("Zebra");
        default :
            return _T("Invalid Animal");
    }
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
    if (IS_LIST_EMPTY(animalListHead)) {
        ConsoleWriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

    CRITICAL_SECTION animalRemovalCrit;

    if (!InitializeCriticalSectionAndSpinCount(&animalRemovalCrit, 4000)) {
        ConsoleWriteLine(_T("Failed to initialize CRITICAL_SECTION: %d\n"), GetLastError());
        return;
    }

    EnterCriticalSection(&animalRemovalCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        AnimalList* tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList);

        if (_tcscmp(animal->UniqueName, tempAnimal->ZooAnimal.UniqueName) == 0) {
            temp->Flink->Blink = temp->Blink;
            temp->Blink->Flink = temp->Flink;

            AddRandomName(animal->UniqueName);
        }

        // TODO: Re-implement
        // HeapFree(GetProcessHeap(), 0, tempAnimal);

        temp = temp->Blink;
    };

    LeaveCriticalSection(&animalRemovalCrit);

    DeleteCriticalSection(&animalRemovalCrit);
}

#pragma endregion

#pragma region Animal Get Functions

void GetAllAnimals() {
    if (IS_LIST_EMPTY(animalListHead)) {
        ConsoleWriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

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
    if (IS_LIST_EMPTY(animalListHead)) {
        ConsoleWriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

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

void GetAllAnimalsInteractivity() {
    if (IS_LIST_EMPTY(animalListHead)) {
        ConsoleWriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

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

        if (tempAnimal.InteractiveLevel >= 6) {
            ConsoleWriteLine(_T("(%c%d%r)\n"), LIME, tempAnimal.InteractiveLevel);
        } else if (tempAnimal.InteractiveLevel >= 3) {
            ConsoleWriteLine(_T("(%c%d%r)\n"), YELLOW, tempAnimal.InteractiveLevel);
        } else {
            ConsoleWriteLine(_T("(%c%d%r)\n"), PINK, tempAnimal.InteractiveLevel);
        }

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);
}

#pragma endregion

#pragma region Animal Change Functions

void DecreaseAnimalFedTimer() {
    if (IS_LIST_EMPTY(animalListHead)) {
        ConsoleWriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

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

void SetHealthEvent(ZooAnimal* animal) {
    if (healthEvent == NULL) {
        healthEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // Temporary, will be moved to NewCage

        if (healthEvent == NULL) {
            ConsoleWriteLine(_T("%cFailed to create event: %d\n"), RED, GetLastError());
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

#pragma endregion

#pragma region Cage Get Functions

BOOL IsCageEmpty(LPTSTR cageName) {
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

DWORD GetCageTotalInteractiveLevel(LPTSTR cageName) {
    DWORD total = 0;

    if (IS_LIST_EMPTY(animalListHead)) {
        ConsoleWriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
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

DWORD GetCageAverageInteractiveLevel(LPTSTR cageName) {
    DWORD total = 0;
    DWORD count = 0;

    if (IS_LIST_EMPTY(animalListHead)) {
        ConsoleWriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
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

Cage* GetRandomCage() {
    srand((unsigned)time(NULL));

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
    cage->AnimalInteractivityThread = CreateThread(NULL, 0, &AnimalInteractivity, NULL, 0, NULL);

    return cage;
}

#pragma endregion

#pragma region Animal Thread Functions

DWORD WINAPI AnimalHealth(LPVOID lpParam) {
    Cage* cage = lpParam;
    HANDLE events[2];

    if (cage->FeedEvent == NULL) {
        ConsoleWriteLine(_T("%cFeed Event is NULL\n"), RED);
        return 1;
    }

    events[0] = cage->FeedEvent;
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

            if (_tcscmp(tempAnimal->CageName, cage->Name) == 0) {
                if (tempAnimal->HealthLevel < 10) {
                    tempAnimal->HealthLevel++;
                    tempAnimal->HealthLevelChange = TRUE;
                }

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
    lpParam = NULL; // Clears W4 warning
    HANDLE events[2];

    events[0] = healthEvent;
    events[1] = appClose;

    do {
        // TODO: Why is this using so much CPU?
        if (WaitForMultipleObjects(2, events, FALSE, INFINITE) == 1) {
            return 0;
        }

        if (IS_LIST_EMPTY(animalListHead)) { continue; }

        EnterCriticalSection(&AnimalListCrit);

        NodeEntry* temp = animalListHead->Blink;

        while (temp != animalListHead) {
            ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

            if (tempAnimal->HealthLevelChange) {
                if (tempAnimal->InteractiveLevel < 10) {
                    tempAnimal->InteractiveLevel++;
                    // TODO: Need to determine if HealthLevel went up or down
                }

                tempAnimal->HealthLevelChange = FALSE;
            }

            temp = temp->Blink;
        };

        LeaveCriticalSection(&AnimalListCrit);
    } while (TRUE);
}

DWORD WINAPI SignificantEventTimer(LPVOID lpParam) {
    lpParam = NULL;

    HANDLE events[2];

    events[0] = significantEventTimer;
    events[1] = appClose;

    do {
        if (WaitForMultipleObjects(2, events, FALSE, INFINITE) == 1) {
            return 0;
        }

        srand((unsigned)time(NULL));

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
                ConsoleWriteLine(
                    _T("%c%s the %s has escaped!\n"),
                    YELLOW,
                    selectedAnimal->UniqueName,
                    AnimalTypeToString(selectedAnimal->AnimalType)
                );
                // Replace 1 with method to call number of visitors
                ConsoleWriteLine(_T("You have lost %d points because all visitors left the zoo...\n"), 1);
                g_Score += 1;

                RemoveAnimal(selectedAnimal);
            } else {
                ConsoleWriteLine(
                    _T("%c%s the %s has given birth to a baby %s!\n"),
                    LIME,
                    selectedAnimal->UniqueName,
                    AnimalTypeToString(selectedAnimal->AnimalType),
                    AnimalTypeToString(selectedAnimal->AnimalType)
                );
                // Replace 1 with method to call number of visitors
                ConsoleWriteLine(_T("All visitors have left for the day and you earned %d points...\n"), 3);
                g_Score += 3;

                // TODO: Need to get a random name and interactive level
                NewAnimal(
                    selectedAnimal->AnimalType,
                    GetRandomName(),
                    selectedAnimal->CageName,
                    GetRandomInteractiveLevel()
                );
            }
        }

        LeaveCriticalSection(&AnimalListCrit);

        if (!SetWaitableTimer(significantEventTimer, &seDueTime, 0, NULL, NULL, FALSE)) {
            ConsoleWriteLine(_T("Failed to set Significant Event Timer: %d\n"), GetLastError());
        }

        EndTurnActions();
    } while (TRUE);
}

#pragma endregion
