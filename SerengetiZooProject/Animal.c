#include "Animals.h"
#include <WriteLine.h>
#include <tchar.h>
#include <ConsoleColors.h>

HANDLE feedEvent;

void InitializeAnimalCriticalSection() {
    
}

ZooAnimal* NewAnimal(enum AnimalType animalType, LPTSTR uniqueName, LPTSTR cageName, DWORD interactiveLevel) {
    ZooAnimal* newAnimal = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ZooAnimal));

    if (newAnimal == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory for new animal: %d"), RED, GetLastError());
        return NULL;
    }

    newAnimal->AnimalType = animalType;
    newAnimal->UniqueName = uniqueName;
    newAnimal->CageName = cageName;
    newAnimal->InteractiveLevel = interactiveLevel;

    return newAnimal;
}

void AddAnimal(CRITICAL_SECTION* cs, NodeEntry* listHead, ZooAnimal* zooAnimal) {
    
    AnimalList* newListItem = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AnimalList));

    if (zooAnimal == NULL || newListItem == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory for new list: %d"), RED, GetLastError());
        return;
    }

    EnterCriticalSection(cs);

    NodeEntry* temp = listHead->Flink;

    newListItem->LinkedList.Flink = temp;
    newListItem->LinkedList.Blink = listHead;
    temp->Blink = &newListItem->LinkedList;
    listHead->Flink = &newListItem->LinkedList;

    newListItem->ZooAnimal = zooAnimal;

    LeaveCriticalSection(cs);
}

DWORD WINAPI AnimalHealth(LPVOID lpParam) {
    feedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (feedEvent == NULL) {
        ConsoleWriteLine(_T("%cFailed to create event: %d"), GetLastError());
        return -1;
    }

    return 0;
}

DWORD WINAPI AnimalInteractivity(LPVOID lpParam) {
    return 0;
}
