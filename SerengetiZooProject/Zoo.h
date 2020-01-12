#pragma once

#include <vector>
#include <wil/resource.h>
#include "Cage.h"

class Zoo {
public:
    static bool IsOpen;

    std::vector<wistd::unique_ptr<Cage>> Cages;

    explicit Zoo(int numberOfCages);

    static void EndTurn();

    void GetAllAnimals();
    void GetAllAnimalsHealth();
    void GetAllAnimalsInteractivity();

    Cage* GetRandomCageNumber();
private:
    wil::critical_section m_cs;
};
