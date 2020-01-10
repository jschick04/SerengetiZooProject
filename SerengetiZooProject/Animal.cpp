#include "Animal.h"

Animal::Animal(::AnimalType animalType, LPCTSTR uniqueName, LPCTSTR cageName) {
    AnimalType = animalType;
    UniqueName = uniqueName;
    CageName = cageName;

    HealthLevel = GetRandomHealthLevel();
    InteractiveLevel = GetRandomInteractiveLevel();
}

int Animal::GetRandomHealthLevel() {
    return rand() % 5 + 4;
}

int Animal::GetRandomInteractiveLevel() {
    return rand() % 5 + 4;
}
