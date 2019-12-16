#ifndef ANIMALS_H
#define ANIMALS_H

#include <Windows.h>
#include "SerengetiZooProject.h"

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
    BOOL HealthLevelChange;
    DWORD InteractiveLevel;
    BOOL InteractivityPrompted;
} ZooAnimal;

void InitializeAnimalCriticalSection();

ZooAnimal* NewAnimal(enum AnimalType animalType, LPTSTR uniqueName, LPTSTR cageName, DWORD interactiveLevel);
void AddAnimal(CRITICAL_SECTION* cs, NodeEntry* listHead, ZooAnimal* animal);

void FeedAnimal(ZooAnimal* animal);

DWORD WINAPI AnimalHealth(LPVOID lpParam);
DWORD WINAPI AnimalInteractivity(LPVOID lpParam);

#endif
