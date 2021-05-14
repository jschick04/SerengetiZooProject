#pragma once

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

    explicit Animal(Cage* cage);

    void SetHealthEvent() const;

    void AddHealthLevel();
    void RemoveHealthLevel();

    void AddInteractiveLevel();
    void RemoveInteractiveLevel();

private:
    static ::AnimalType GetRandomAnimalType();
    static int GetRandomHealthLevel();
    static int GetRandomInteractiveLevel();
};
