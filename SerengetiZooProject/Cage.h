#pragma once

#include <vector>
#include <wil/resource.h>
#include <Windows.h>
#include "Animal.h"

class Cage {
public:
    LPCTSTR Name;
    std::vector<Animal> Animals;

    explicit Cage(LPCTSTR name);

    bool IsCageEmpty();

    DWORD GetAverageInteractiveLevel();
    DWORD GetTotalInteractiveLevel();

    Cage GetRandom();

    void AddAnimal();
    void GetAnimals();
    void GetAnimalsHealth();
    void GetAnimalsInteractivity();
    void RemoveAnimal();

private:
    wil::unique_event_failfast m_feedEvent;
    wil::unique_handle m_feedEventTimer;
    wil::unique_handle m_animalHealthThread;
    wil::unique_handle m_animalInteractivityThread;

    DWORD WINAPI AnimalHealth(LPVOID lpParam);
    DWORD WINAPI AnimalInteractivity(LPVOID lpParam);
    DWORD WINAPI SignificantEventTimer(LPVOID);
};
