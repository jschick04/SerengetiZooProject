#pragma once

#include <vector>
#include <wil/resource.h>
#include "Cage.h"
#include "SignificantEvent.h"

class Zoo {
public:
    static bool IsOpen;

    std::vector<wistd::unique_ptr<Cage>> Cages;
    wistd::unique_ptr<SignificantEvent> Event;

    explicit Zoo(int numberOfCages);

    static void EndTurn();

    bool IsZooEmpty();

    void GetAllAnimals();
    void GetAllAnimalsHealth();
    void GetAllAnimalsInteractivity();

    Cage* GetRandomCage();
private:
    wil::critical_section m_cs;
};
