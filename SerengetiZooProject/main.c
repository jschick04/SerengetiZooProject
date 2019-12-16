#include <Windows.h>
#include <tchar.h>
#include "WriteLine.h"
#include "ConsoleColors.h"
#include "Animals.h"
#include "Visitor.h"

enum AnimalType { Antelopes, Giraffes, Hyaena, Hippos, Monkeys, Mongoose, Tigers, WildeBeast, Zebras };
enum VisitorStatus { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

typedef struct ZooAnimal {
    enum AnimalType AnimalType;
    LPTSTR UniqueName;
    LPTSTR CageName;
    DWORD HealthLevel;
    BOOL HealthLevelChange;
    DWORD InteractiveLevel;
    BOOL InteractivityPrompted;
} ZooAnimal;

typedef struct Visitor {
    LPTSTR UniqueName;
    LPTSTR CageLocation;
    DWORD HappinessLevel;
    enum VisitorStatus Status;
    struct NodeEntry Links;
} Visitor;


CRITICAL_SECTION AnimalListCrit;
NodeEntry* animalListHead = 0;
NodeEntry* visitorListHead = 0;

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
