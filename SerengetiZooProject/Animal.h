#pragma once

#include <wil/resource.h>
#include <Windows.h>

enum class AnimalType { Antelope, Cheetah, Giraffe, Hyaena, Hippo, Monkey, Mongoose, Tiger, WildeBeast, Zebra };

class Animal {
public:
    AnimalType AnimalType;
    LPCTSTR UniqueName;
    LPCTSTR CageName;
    DWORD HealthLevel;
    BOOL HealthLevelChange = false;
    DWORD InteractiveLevel;
    BOOL InteractivityPrompted = false;

    static wil::critical_section CriticalSection;

    Animal(const ::AnimalType animalType, const TCHAR* uniqueName, const TCHAR* cageName);

private:
    static int GetRandomHealthLevel();
    static int GetRandomInteractiveLevel();

    void AddHealthLevel();
    void RemoveHealthLevel();
    void SetHealthEvent();

    void AddInteractiveLevel();
    void RemoveInteractiveLevel();

    void ResetFeedTime();
};
