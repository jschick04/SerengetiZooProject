#include "SerengetiZooProject.h"
#include "Animals.h"
#include <WriteLine.h>
#include <tchar.h>
#include <ConsoleColors.h>

HANDLE healthEvent;

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

    newListItem->ZooAnimal = animal;

    LeaveCriticalSection(&AnimalListCrit);
}

//void GetAllAnimals() {
//    AnimalList* temp = CONTAINING_RECORD(animalListHead->Flink, AnimalList, );
//
//    do {
//        temp = temp->LinkedList;
//        ZooAnimal tempAnimal = CONTAINING_RECORD(temp, ZooAnimal*, )
//    } while (temp->Flink != animalListHead);
//}

DWORD WINAPI AnimalHealth(LPVOID lpParam) {
    feedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (feedEvent == NULL) {
        ConsoleWriteLine(_T("%cFailed to create event: %d\n"), GetLastError());
        return -1;
    }

    do {
        WaitForSingleObject(feedEvent, INFINITE);

        EnterCriticalSection(&AnimalListCrit);

        // TODO: What are we searching for?

        //ConsoleWriteLine(_T("%s the %s has been fed\n"), name, type);

        LeaveCriticalSection(&AnimalListCrit);
    } while (TRUE);

    return 0;
}

DWORD WINAPI AnimalInteractivity(LPVOID lpParam) {
    return 0;
}
