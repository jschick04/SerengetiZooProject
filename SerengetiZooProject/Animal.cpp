#include "Animal.h"

#include <ConsoleColors.h>
#include <cwl.h>
#include <random>
#include <tchar.h>
#include "Cage.h"
#include "GameManager.h"
#include "Helpers.h"

wil::critical_section Animal::CriticalSection;

Animal::Animal(Cage* cage) {
    AnimalType = GetRandomAnimalType();
    UniqueName = Helpers::GetRandomName();
    CurrentCage = cage;

    HealthLevel = GetRandomHealthLevel();
    InteractiveLevel = GetRandomInteractiveLevel();
}

void Animal::AddHealthLevel() {
    auto guard = CriticalSection.lock();

    const DWORD healthChange = rand() % (GameManager::FeedLevelMax - 1) + GameManager::FeedLevelMin;
    const auto newValue = HealthLevel + healthChange;

    if (HealthLevel >= 10) {
        cwl::WriteLine(
            _T("%c%s the %s is already full...\n"),
            LIME,
            UniqueName,
            Helpers::AnimalTypeToString(AnimalType)
        );
        return;
    }

    if (newValue >= 10) {
        HealthLevel = 10;
    } else {
        HealthLevel = newValue;
    }

    HealthLevelChange = TRUE;
    InteractivityPrompted = TRUE;

    cwl::WriteLine(_T("%c%s the %s has been fed\n"), LIME, UniqueName, Helpers::AnimalTypeToString(AnimalType));

    SetHealthEvent();
}

void Animal::RemoveHealthLevel() {
    auto guard = CriticalSection.lock();

    const auto newValue = HealthLevel - GameManager::HungerLevel;

    if (newValue <= 0) {
        HealthLevel = 0;
    } else {
        HealthLevel = newValue;
    }

    HealthLevelChange = TRUE;
    InteractivityPrompted = FALSE;

    if (HealthLevel < 5) {
        cwl::WriteLine(_T("%c%s the %s is hungry\n"), YELLOW, UniqueName, Helpers::AnimalTypeToString(AnimalType));
    }

    SetHealthEvent();
}

void Animal::SetHealthEvent() const {
    if (HealthLevel >= 1 || HealthLevel < 5) {
        cwl::WriteLine(_T("%s the %s is %csick\n"), UniqueName, Helpers::AnimalTypeToString(AnimalType), PINK);

        CurrentCage->HealthEvent.SetEvent();
    }

    if (HealthLevel < 1) {
        cwl::WriteLine(
            _T("\n%c%s the %s is seriously ill and the Zoo Oversight Committee has relocated the animal\n\n"),
            PINK,
            UniqueName,
            Helpers::AnimalTypeToString(AnimalType)
        );

        GameManager::Score -= 3;

        CurrentCage->RemoveAnimal(UniqueName);
    }
}

void Animal::AddInteractiveLevel() {
    auto guard = CriticalSection.lock();

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
    auto guard = CriticalSection.lock();

    const auto newValue = InteractiveLevel - GameManager::HungerLevel;

    if (InteractiveLevel <= 0) { return; }

    if (newValue <= 0) {
        InteractiveLevel = 0;
    } else {
        InteractiveLevel = newValue;
    }
}

::AnimalType Animal::GetRandomAnimalType() {
    std::random_device generator;
    const std::uniform_int_distribution<int> range(0, 9);

    return ::AnimalType(range(generator));
}

int Animal::GetRandomHealthLevel() {
    std::random_device generator;
    const std::uniform_int_distribution<int> range(5, 8);

    return range(generator);
}

int Animal::GetRandomInteractiveLevel() {
    std::random_device generator;
    const std::uniform_int_distribution<int> range(5, 8);

    return range(generator);
}
