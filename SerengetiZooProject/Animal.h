#pragma once

#include <wil/resource.h>
#include <Windows.h>

class Cage;

enum class AnimalType { Antelope, Cheetah, Giraffe, Hyaena, Hippo, Monkey, Mongoose, Tiger, WildeBeast, Zebra };

class Animal {
public:
    AnimalType AnimalType;
    LPCTSTR UniqueName;
    Cage* CurrentCage;
    DWORD HealthLevel;
    BOOL HealthLevelChange = false;
    DWORD InteractiveLevel;
    BOOL InteractivityPrompted = false;

    static wil::critical_section CriticalSection;

    Animal(::AnimalType animalType, const TCHAR* uniqueName, Cage* cage);

    void SetHealthEvent();

    void AddHealthLevel();
    void RemoveHealthLevel();

    void AddInteractiveLevel();
    void RemoveInteractiveLevel();

private:
    static int GetRandomHealthLevel();
    static int GetRandomInteractiveLevel();
};
