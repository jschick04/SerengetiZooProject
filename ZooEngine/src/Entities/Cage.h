#pragma once

#include "Animal.h"

#ifdef UNICODE
typedef std::wostringstream _tstringstream;
#else
typedef std::ostringstream _tstringstream;
#endif

namespace SerengetiZoo
{
    class Cage
    {
    public:
        explicit Cage(int number);

    public:
        void AddAnimal();
        void FeedAnimals() const;
        void GetAnimals();
        void GetAnimalsHealth();
        void GetAnimalsInteractiveLevel();
        Animal* GetRandomAnimal();
        void RemoveAnimal(const Animal& animal);

        [[nodiscard]] DWORD GetAverageInteractiveLevel();
        [[nodiscard]] DWORD GetTotalInteractiveLevel();

        [[nodiscard]] LPCTSTR GetName() const noexcept { return m_name; }

        [[nodiscard]] LPCTSTR GetType() const noexcept { return ToString(m_cageType); }

        [[nodiscard]] bool IsCageEmpty();

        void WaitForThreads() const noexcept;

    private:
        static DWORD WINAPI AnimalHealth(LPVOID lpParam);
        static DWORD WINAPI AnimalInteractivity(LPVOID);

        static LPCTSTR GetCageName(int number);

    private:
        void ResetFeedTimer(HANDLE feedEventTimer) const;

    private:
        wil::critical_section m_cs;

        std::vector<Animal> m_animals;
        AnimalType m_cageType;
        LPCTSTR m_name;

    private:
        LARGE_INTEGER m_feedDueTime;
        wil::unique_event m_feedEvent;
        wil::unique_handle m_feedEventTimer;
        wil::unique_event m_healthEvent;

    private:
        wil::unique_handle m_animalHealthThread;
        wil::unique_handle m_animalInteractivityThread;
    };
}
