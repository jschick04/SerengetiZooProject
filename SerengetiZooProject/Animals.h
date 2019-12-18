#ifndef ANIMALS_H
#define ANIMALS_H

#include <Windows.h>
#include "SerengetiZooProject.h"

enum AnimalType { Antelopes, Cheetahs, Giraffes, Hyaena, Hippos, Monkeys, Mongoose, Tigers, WildeBeast, Zebras };

typedef struct Cage {
    LPTSTR Name;
    HANDLE FeedEvent;
    HANDLE AnimalHealthThread;
    HANDLE AnimalInteractivityThread;
} Cage;

typedef struct ZooAnimal {
    enum AnimalType AnimalType;
    LPTSTR UniqueName;
    LPTSTR CageName;
    DWORD HealthLevel;
    BOOL HealthLevelChange;
    DWORD InteractiveLevel;
    BOOL InteractivityPrompted;
} ZooAnimal;

typedef struct AnimalList {
    ZooAnimal ZooAnimal;
    struct NodeEntry LinkedList;
} AnimalList;

// Defines the number of cages that will be in the zoo
Cage* cages[5];

ZooAnimal* NewAnimal(enum AnimalType animalType, LPTSTR uniqueName, LPTSTR cageName, DWORD interactiveLevel);
void AddAnimal(ZooAnimal* animal);
void RemoveAnimal(ZooAnimal* animal);

void GetAllAnimals();
void GetAllAnimalsHealth();

Cage* NewCage(LPTSTR cageName);

DWORD GetCageTotalInteractiveLevel(LPTSTR cageName);
DWORD GetCageAverageInteractiveLevel(LPTSTR cageName);

DWORD WINAPI AnimalHealth(LPVOID lpParam);
DWORD WINAPI AnimalInteractivity(LPVOID lpParam);

#endif
