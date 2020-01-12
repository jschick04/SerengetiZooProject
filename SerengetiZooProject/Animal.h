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

    Animal(const ::AnimalType animalType, const TCHAR* uniqueName, const TCHAR* cageName);

private:
    wil::unique_event_failfast m_healthEvent;

    static int GetRandomHealthLevel();
    static int GetRandomInteractiveLevel();

    void AddHealthLevel();
    void RemoveHealthLevel();
    void SetHealthEvent();

    void AddInteractiveLevel();
    void RemoveInteractiveLevel();

    void ResetFeedTime();
};