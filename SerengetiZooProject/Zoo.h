#pragma once

#include <vector>
#include <wil/resource.h>
#include "Cage.h"

class Zoo {
public:
    static int Score;
    static bool IsOpen;

    std::vector<wistd::unique_ptr<Cage>> Cages;

    explicit Zoo(int numberOfCages);

    void EndTurn();
private:
    wil::critical_section m_cs;
};
