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

void Animal::AddHealthLevel() {
    const DWORD healthChange = rand() % (FEED_LEVEL_MAX - 1) + FEED_LEVEL_MIN;
    const DWORD newValue = animal->HealthLevel + healthChange;

    if (animal->HealthLevel >= 10) {
        cwl::WriteLine(
            _T("%c%s the %s is already full...\n"),
            LIME,
            animal->UniqueName,
            AnimalTypeToString(animal->AnimalType)
        );
        return;
    }

    if (newValue >= 10) {
        animal->HealthLevel = 10;
    } else {
        animal->HealthLevel = newValue;
    }

    animal->HealthLevelChange = TRUE;
    animal->InteractivityPrompted = TRUE;

    cwl::WriteLine(
        _T("%c%s the %s has been fed\n"),
        LIME,
        animal->UniqueName,
        AnimalTypeToString(animal->AnimalType)
    );
}

void Animal::RemoveHealthLevel() {
    const DWORD newValue = animal->HealthLevel - HUNGER_LEVEL;

    if (newValue <= 0) {
        animal->HealthLevel = 0;
    } else {
        animal->HealthLevel = newValue;
    }

    animal->HealthLevelChange = TRUE;
    animal->InteractivityPrompted = FALSE;

    if (animal->HealthLevel < 5) {
        cwl::WriteLine(
            _T("%c%s the %s is hungry\n"),
            YELLOW,
            animal->UniqueName,
            AnimalTypeToString(animal->AnimalType)
        );
    }
}

void Animal::SetHealthEvent() {
    if (animal->HealthLevel < 1) {
        cwl::WriteLine(
            _T("\n%c%s the %s is seriously ill and the Zoo Oversight Committee has relocated the animal\n\n"),
            PINK,
            animal->UniqueName,
            AnimalTypeToString(animal->AnimalType)
        );

        RemoveAnimal(animal);

        g_Score -= 3;
    } else if (animal->HealthLevel < 5) {
        cwl::WriteLine(_T("%s the %s is %csick\n"), animal->UniqueName, AnimalTypeToString(animal->AnimalType), PINK);
    }

    SetEvent(healthEvent);
}

void Animal::AddInteractiveLevel() {
    const DWORD interactiveChange = rand() % (FEED_LEVEL_MAX - 1) + FEED_LEVEL_MIN;
    const DWORD newValue = animal->InteractiveLevel + interactiveChange;

    if (animal->InteractiveLevel >= 10) { return; }

    if (newValue >= 10) {
        animal->InteractiveLevel = 10;
    } else {
        animal->InteractiveLevel = newValue;
    }
}

void Animal::RemoveInteractiveLevel() {
    const DWORD newValue = animal->InteractiveLevel - HUNGER_LEVEL;

    if (animal->InteractiveLevel <= 0) { return; }

    if (newValue <= 0) {
        animal->InteractiveLevel = 0;
    } else {
        animal->InteractiveLevel = newValue;
    }
}

void Animal::ResetFeedTime() {
    if (SetWaitableTimer(eventTimer, &feedDueTime, 0, NULL, NULL, FALSE)) {
        return TRUE;
    }

    cwl::WriteLine(_T("Failed to set Feed Event Timer: %d\n"), GetLastError());

    return FALSE;
}
