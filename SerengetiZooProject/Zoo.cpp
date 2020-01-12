#include "Zoo.h"
#include <ConsoleColors.h>
#include <cwl.h>
#include <tchar.h>
#include "GameManager.h"
#include "Helpers.h"
#include "Menu.h"

bool Zoo::IsOpen = false;

Zoo::Zoo(const int numberOfCages) {
    GameManager::Score = 0;

    for (int i = 1; i <= numberOfCages; i++) {
        auto cage = wil::make_unique_failfast<Cage>(i);
        auto animal = wil::make_unique_failfast<Animal>(AnimalType(i), Helpers::GetRandomName(), cage->Name);

        cage->AddAnimal(move(animal));
        Cages.push_back(move(cage));
    }
}

// Closes the zoo for the day and tells visitors to leave
void Zoo::EndTurn() {
    cwl::WriteLine(_T("\n%cZoo is closing for the rest of the day...\n"), PINK);

    // TODO: Reimplement
    /*ExitZoo();
    int ThreadHandleCount = VisitorTID;
    DWORD ret = WaitForMultipleObjects(ThreadHandleCount, hThreadHandles, TRUE, INFINITE);
    if (ret == 0xFFFFFFFF) {
        DWORD returnstring = GetLastError();
        cwl::WriteLine(_T("Visitor Wait failed with %d"), returnstring);
    }*/
    IsOpen = false;

    // TODO: ResetZooClosedTimer();

    Menu::PrintScore();
    Menu::PrintCurrentZooStatus();
    Menu::PrintOptions();
}

// Checks all cages and returns if all cages are empty
bool Zoo::IsZooEmpty() {
    int count = 0;

    auto guard = m_cs.lock();

    for (auto const& cage : Cages) {
        if (cage->IsCageEmpty()) {
            count++;
        }
    }

    return count > 0;
}

// Prints all animals
void Zoo::GetAllAnimals() {
    auto lock = m_cs.lock();

    for (auto const& cage : Cages) {
        for (auto const& animal : cage->Animals) {
            cwl::WriteLine(
                _T("[%c%s%r] %s the %s\n"),
                SKYBLUE,
                animal->CageName,
                animal->UniqueName,
                Helpers::AnimalTypeToString(animal->AnimalType)
            );
        }
    }
}

// Prints all animals health values
void Zoo::GetAllAnimalsHealth() {
    auto lock = m_cs.lock();

    for (auto const& cage : Cages) {
        if (cage->IsCageEmpty()) {
            cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage->Name);
            continue;
        }

        for (auto const& animal : cage->Animals) {
            cwl::WriteLine(
                _T("[%c%s%r] %s the %s "),
                SKYBLUE,
                animal->CageName,
                animal->UniqueName,
                Helpers::AnimalTypeToString(animal->AnimalType)
            );

            if (animal->HealthLevel >= 6) {
                cwl::WriteLine(_T("(%c%d%r)\n"), LIME, animal->HealthLevel);
            } else if (animal->HealthLevel >= 3) {
                cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, animal->HealthLevel);
            } else {
                cwl::WriteLine(_T("(%c%d%r)\n"), PINK, animal->HealthLevel);
            }
        }
    }
}

// Prints all animals interactivity values
void Zoo::GetAllAnimalsInteractivity() {
    auto lock = m_cs.lock();

    for (auto const& cage : Cages) {
        if (cage->IsCageEmpty()) {
            cwl::WriteLine(_T("%c[%s] No Animals In The Cage!\n"), PINK, cage->Name);
            continue;
        }

        for (auto const& animal : cage->Animals) {
            cwl::WriteLine(
                _T("[%c%s%r] %s the %s "),
                SKYBLUE,
                animal->CageName,
                animal->UniqueName,
                Helpers::AnimalTypeToString(animal->AnimalType)
            );

            if (animal->InteractiveLevel >= 6) {
                cwl::WriteLine(_T("(%c%d%r)\n"), LIME, animal->InteractiveLevel);
            } else if (animal->InteractiveLevel >= 3) {
                cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, animal->InteractiveLevel);
            } else {
                cwl::WriteLine(_T("(%c%d%r)\n"), PINK, animal->InteractiveLevel);
            }
        }
    }
}

// Returns a random cage pointer
Cage* Zoo::GetRandomCage() {
    std::vector<Cage*> availableCages;

    auto guard = m_cs.lock();

    for (auto const& cage : Cages) {
        if (!cage->IsCageEmpty()) {
            availableCages.push_back(cage.get());
        }
    }

    return availableCages.empty() ? nullptr : availableCages.at(rand() % availableCages.size());
}
