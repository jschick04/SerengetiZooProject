#pragma once

#include <sstream>
#include <vector>
#include <wil/resource.h>
#include <Windows.h>
#include "Animal.h"

#ifdef UNICODE
typedef std::wostringstream _tstringstream;
#else
typedef std::ostringstream -tstringstream;
#endif

class Cage {
public:
    LPCTSTR Name;
    std::vector<wistd::unique_ptr<Animal>> Animals;
    wil::unique_event HealthEvent;

    explicit Cage(int number);

    bool IsCageEmpty() noexcept;

    DWORD GetAverageInteractiveLevel() noexcept;
    DWORD GetTotalInteractiveLevel() noexcept;

    void AddAnimal(wistd::unique_ptr<Animal> animal);
    void RemoveAnimal(wistd::unique_ptr<Animal> animal);

    void WaitForThreads() const noexcept;
private:
    wil::critical_section m_cs;

    wil::unique_event m_feedEvent;
    wil::unique_handle m_feedEventTimer;
    wil::unique_handle m_animalHealthThread;
    wil::unique_handle m_animalInteractivityThread;

    static LPCTSTR GetCageName(int number);

    static DWORD WINAPI AnimalHealth(LPVOID);
    static DWORD WINAPI AnimalInteractivity(LPVOID);
};
