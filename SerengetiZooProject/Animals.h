#ifndef ANIMALS_H
#define ANIMALS_H

#include <Windows.h>
#include "SerengetiZooProject.h"

#define SIGNIFICANT_EVENT_MIN 2
#define FEED_EVENT_MIN 1
#define MAX_CAGES 6

HANDLE significantEventTimer;

enum AnimalType { Antelope, Cheetah, Giraffe, Hyaena, Hippo, Monkey, Mongoose, Tiger, WildeBeast, Zebra };

typedef struct Cage {
    LPTSTR Name;
    HANDLE FeedEvent;
    HANDLE FeedEventTimer;
    HANDLE AnimalHealthThread;
    HANDLE AnimalInteractivityThread;
} Cage;

typedef struct ZooAnimal {
    enum AnimalType AnimalType;
    LPTSTR UniqueName;
    LPTSTR CageName;
    DWORD HealthLevel;
    BOOL HealthLevelChange;
    BOOL HealthLevelIncrease;
    DWORD InteractiveLevel;
    BOOL InteractivityPrompted;
} ZooAnimal;

typedef struct AnimalList {
    ZooAnimal ZooAnimal;
    NodeEntry LinkedList;
} AnimalList;

// Defines the number of cages that will be in the zoo
Cage* cages[MAX_CAGES];

// Defines the timer for the Significant Event Thread
LARGE_INTEGER seDueTime;
LARGE_INTEGER feedDueTime;

void NewAnimal(enum AnimalType animalType, LPTSTR uniqueName, LPTSTR cageName, DWORD interactiveLevel);
void AddAnimal(ZooAnimal* animal);
void RemoveAnimal(ZooAnimal* animal);

void GetAllAnimals();
void GetAllAnimalsHealth();
void GetAllAnimalsInteractivity();

Cage* NewCage(LPTSTR cageName);

BOOL IsCageEmpty(LPTSTR cageName);
DWORD GetCageTotalInteractiveLevel(LPTSTR cageName);
DWORD GetCageAverageInteractiveLevel(LPTSTR cageName);

DWORD WINAPI SignificantEventTimer(LPVOID lpParam);

#endif
