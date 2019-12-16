#include <Windows.h>
#include <tchar.h>

enum AnimalType { Antelopes, Giraffes, Hyaena, Hippos, Monkeys, Mongoose, Tigers, WildeBeast, Zebras };

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
        return -1;
    }

    if (InitializeCriticalSectionAndSpinCount(&AnimalListCrit, 4000) == FALSE) {
        return -1;
    }

    // TODO: Implement Main Loop

    HeapFree(GetProcessHeap(), 0, &animalListHead);
    HeapFree(GetProcessHeap(), 0, &visitorListHead);
}
