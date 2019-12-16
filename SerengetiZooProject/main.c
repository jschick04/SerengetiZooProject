#include <Windows.h>
#include <tchar.h>
#include <WriteLine.h>
#include <ConsoleColors.h>
#include "Visitor.h"

#define IS_LIST_EMPTY(listHead) ((listHead)->blink == (listHead));

NodeEntry* animalListHead = 0;
NodeEntry* visitorListHead = 0;

CRITICAL_SECTION AnimalListCrit;

BOOL InitializeListHead() {
    animalListHead = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    visitorListHead = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));

    if (animalListHead == 0 || visitorListHead == 0) {
        return FALSE;
    }

    animalListHead->Flink = animalListHead->Blink = animalListHead;
    visitorListHead->Flink = visitorListHead->Blink = visitorListHead;

    return TRUE;
}

int _tmain() {
    if (InitializeListHead() == FALSE) {
        ConsoleWriteLine(_T("%cFailed to create list heads"), RED);
        return -1;
    }

    if (InitializeCriticalSectionAndSpinCount(&AnimalListCrit, 4000) == FALSE) {
        ConsoleWriteLine(_T("%cFailed to create CRITICAL_SECTION"), RED);
        return -1;
    }

    // TODO: Implement Main Loop

    HeapFree(GetProcessHeap(), 0, &animalListHead);
    HeapFree(GetProcessHeap(), 0, &visitorListHead);
}
