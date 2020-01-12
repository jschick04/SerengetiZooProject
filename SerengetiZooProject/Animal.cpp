#include "Animal.h"
#include <ConsoleColors.h>
#include <cwl.h>
#include <tchar.h>
#include "GameManager.h"

Animal::Animal(const ::AnimalType animalType, const TCHAR* uniqueName, const TCHAR* cageName) {
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
    const DWORD healthChange = rand() % (GameManager::FeedLevelMax - 1) + GameManager::FeedLevelMin;
    const auto newValue = HealthLevel + healthChange;

    if (HealthLevel >= 10) {
        cwl::WriteLine(_T("%c%s the %s is already full...\n"), LIME, UniqueName, AnimalType);
        return;
    }

    if (newValue >= 10) {
        HealthLevel = 10;
    } else {
        HealthLevel = newValue;
    }

    HealthLevelChange = TRUE;
    InteractivityPrompted = TRUE;

    cwl::WriteLine(_T("%c%s the %s has been fed\n"), LIME, UniqueName, AnimalType);
}

void Animal::RemoveHealthLevel() {
    const auto newValue = HealthLevel - GameManager::HungerLevel;

    if (newValue <= 0) {
        HealthLevel = 0;
    } else {
        HealthLevel = newValue;
    }

    HealthLevelChange = TRUE;
    InteractivityPrompted = FALSE;

    if (HealthLevel < 5) {
        cwl::WriteLine(_T("%c%s the %s is hungry\n"), YELLOW, UniqueName, AnimalType);
    }
}

void Animal::SetHealthEvent() {
    if (HealthLevel < 1) {
        cwl::WriteLine(
            _T("\n%c%s the %s is seriously ill and the Zoo Oversight Committee has relocated the animal\n\n"),
            PINK,
            UniqueName,
            AnimalType
        );

        // TODO: Add this back
        //Cage.RemoveAnimal(this);

        GameManager::Score -= 3;
    } else if (HealthLevel < 5) {
        cwl::WriteLine(_T("%s the %s is %csick\n"), UniqueName, AnimalType, PINK);
    }

    //m_healthEvent.SetEvent();
}

void Animal::AddInteractiveLevel() {
    const DWORD interactiveChange = rand() % (GameManager::FeedLevelMax - 1) + GameManager::FeedLevelMin;
    const auto newValue = InteractiveLevel + interactiveChange;

    if (InteractiveLevel >= 10) { return; }

    if (newValue >= 10) {
        InteractiveLevel = 10;
    } else {
        InteractiveLevel = newValue;
    }
}

void Animal::RemoveInteractiveLevel() {
    const auto newValue = InteractiveLevel - GameManager::HungerLevel;

    if (InteractiveLevel <= 0) { return; }

    if (newValue <= 0) {
        InteractiveLevel = 0;
    } else {
        InteractiveLevel = newValue;
    }
}

void Animal::ResetFeedTime() {
    // TODO: Implement back
    /*if (SetWaitableTimer(eventTimer, &feedDueTime, 0, NULL, NULL, FALSE)) {
        return TRUE;
    }

    cwl::WriteLine(_T("Failed to set Feed Event Timer: %d\n"), GetLastError());

    return FALSE;*/
}
