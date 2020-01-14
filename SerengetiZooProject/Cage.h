#pragma once

#include <sstream>
#include <vector>
#include <wil/resource.h>
#include <Windows.h>
#include "Animal.h"

#ifdef UNICODE
typedef std::wostringstream _tstringstream;
#else
typedef std::ostringstream _tstringstream;
#endif

class Cage {
public:
    LPCTSTR Name;
    std::vector<wistd::unique_ptr<Animal>> Animals;
    wil::unique_event FeedEvent;
    wil::unique_event HealthEvent;

    static wil::critical_section CriticalSection;

    explicit Cage(int number);

    bool IsCageEmpty() const noexcept;

    DWORD GetAverageInteractiveLevel() const noexcept;
    DWORD GetTotalInteractiveLevel() const noexcept;

    void AddAnimal(wistd::unique_ptr<Animal> animal);
    void RemoveAnimal(LPCTSTR uniqueName);

    void WaitForThreads() const noexcept;

private:
    LARGE_INTEGER m_feedDueTime;
    wil::unique_handle m_feedEventTimer;
    wil::unique_handle m_animalHealthThread;
    wil::unique_handle m_animalInteractivityThread;

    static LPCTSTR GetCageName(int number);

    void ResetFeedTimer() const;

    static DWORD WINAPI AnimalHealth(LPVOID);
    static DWORD WINAPI AnimalInteractivity(LPVOID);
};
