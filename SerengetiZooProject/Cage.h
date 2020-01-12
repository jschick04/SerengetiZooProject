#pragma once

#include <vector>
#include <wil/resource.h>
#include <Windows.h>
#include "Animal.h"
#include <sstream>

#ifdef UNICODE
typedef std::wostringstream _tstringstream;
#else
typedef std::ostringstream -tstringstream;
#endif

class Cage {
public:
    LPCTSTR Name;
    std::vector<wistd::unique_ptr<Animal>> Animals;

    explicit Cage(const int number);

    bool IsCageEmpty();

    DWORD GetAverageInteractiveLevel();
    DWORD GetTotalInteractiveLevel();

    void AddAnimal(wistd::unique_ptr<Animal> animal);
    void RemoveAnimal(wistd::unique_ptr<Animal> animal);

    void WaitForThreads() const noexcept;
private:
    wil::critical_section m_cs;

    wil::unique_event_failfast m_feedEvent;
    //wil::unique_handle m_feedEventTimer;
    //wil::unique_handle m_animalHealthThread;
    //wil::unique_handle m_animalInteractivityThread;

    static LPCTSTR GetCageName(int number);

    DWORD WINAPI AnimalHealth(LPVOID);
    DWORD WINAPI AnimalInteractivity(LPVOID);
    DWORD WINAPI SignificantEventTimer(LPVOID);
};
