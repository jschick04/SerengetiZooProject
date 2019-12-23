#ifndef ANIMALS_H
#define ANIMALS_H

#include <Windows.h>
#include "SerengetiZooProject.h"

#define SIGNIFICANT_EVENT_MIN 1

HANDLE significantEventTimer;

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
    DWORD FeedTimer;
} ZooAnimal;

typedef struct AnimalList {
    ZooAnimal ZooAnimal;
    struct NodeEntry LinkedList;
} AnimalList;

// Defines the number of cages that will be in the zoo
Cage* cages[5];

// Defines the timer for the Significant Event Thread
LARGE_INTEGER seDueTime;

ZooAnimal* NewAnimal(enum AnimalType animalType, LPTSTR uniqueName, LPTSTR cageName, DWORD interactiveLevel);
void AddAnimal(ZooAnimal* animal);
void RemoveAnimal(ZooAnimal* animal);

void GetAllAnimals();
void GetAllAnimalsHealth();
void GetAllAnimalsInteractivity();

void DecreaseAnimalFedTimer();

Cage* NewCage(LPTSTR cageName);

BOOL IsCageEmpty(LPTSTR cageName);
DWORD GetCageTotalInteractiveLevel(LPTSTR cageName);
DWORD GetCageAverageInteractiveLevel(LPTSTR cageName);

DWORD WINAPI SignificantEventTimer(LPVOID lpParam);

#endif
