#ifndef ANIMALS_H
#define ANIMALS_H

#include <Windows.h>

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

enum AnimalType { Antelopes, Giraffes, Hyaena, Hippos, Monkeys, Mongoose, Tigers, WildeBeast, Zebras };

typedef struct AnimalList {
    struct ZooAnimal* ZooAnimal;
    struct NodeEntry LinkedList;
} AnimalList;

typedef struct ZooAnimal {
    enum AnimalType AnimalType;
    LPTSTR UniqueName;
    LPTSTR CageName;
    DWORD HealthLevel;
    BOOL HealthLevelChange = FALSE;
    DWORD InteractiveLevel;
    BOOL InteractivityPrompted = FALSE;
} ZooAnimal;

ZooAnimal* NewAnimal(enum AnimalType animalType, LPTSTR uniqueName, LPTSTR cageName, DWORD interactiveLevel);
void AddAnimal(CRITICAL_SECTION* cs, NodeEntry* listHead);

#endif
