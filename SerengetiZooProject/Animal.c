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

    EnterCriticalSection(&AnimalListCrit);

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

    LeaveCriticalSection(&AnimalListCrit);
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
            _T("Type: %s, Name: %s, Cage: %s\n"),
            AnimalTypeToString(tempAnimal.AnimalType),
            tempAnimal.UniqueName,
            tempAnimal.CageName
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
            _T("Cage: %s Name: %s Health: %s\n"),
            tempAnimal.CageName,
            tempAnimal.UniqueName,
            tempAnimal.HealthLevel
        );

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

    if (cage->FeedEvent == NULL) {
        ConsoleWriteLine(_T("%cFailed to create event: %d\n"), GetLastError());
        return NULL;
    }

    cage->Name = cageName;
    cage->FeedEvent = feedEvent;
    cage->AnimalHealthThread = CreateThread(NULL, 0, &AnimalHealth, &feedEvent, 0, NULL);

    return cage;
}

#pragma endregion

#pragma region Animal Thread Functions

DWORD WINAPI AnimalHealth(LPVOID lpParam) {
    const Cage cage = *(Cage*)lpParam;

    if (cage.FeedEvent == NULL) {
        ConsoleWriteLine(_T("%cFeed Event is NULL"), RED);
        return 1;
    }

    do {
        WaitForSingleObject(cage.FeedEvent, INFINITE);

        if (IS_LIST_EMPTY(animalListHead)) { continue; }

        EnterCriticalSection(&AnimalListCrit);

        NodeEntry* temp = animalListHead->Blink;

        while (temp != animalListHead) {
            ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

            if (_tcscmp(tempAnimal->CageName, cage.Name) == 0) {
                tempAnimal->HealthLevel++;

                ConsoleWriteLine(
                    _T("%s the %s has been fed"),
                    tempAnimal->UniqueName,
                    AnimalTypeToString(tempAnimal->AnimalType)
                );
            }

            temp = temp->Blink;
        };

        LeaveCriticalSection(&AnimalListCrit);
    } while (TRUE); // Need a break condition

    return 0;
}

DWORD WINAPI AnimalInteractivity(LPVOID lpParam) {
    return 0;
}

#pragma endregion
