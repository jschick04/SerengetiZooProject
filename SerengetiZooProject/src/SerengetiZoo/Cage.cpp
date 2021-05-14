#include "szpch.h"

#include "Cage.h"

wil::critical_section Cage::CriticalSection;

LARGE_INTEGER Cage::m_feedDueTime;

Cage::Cage(const int number) {
    Name = GetCageName(number);

    auto animal = wil::make_unique_failfast<Animal>(this);
    THROW_LAST_ERROR_IF_NULL(animal);

    Animals.push_back(move(animal));

    FeedEvent.create(wil::EventOptions::None);
    THROW_LAST_ERROR_IF(!FeedEvent.is_valid());

    m_feedEventTimer.reset(CreateWaitableTimer(nullptr, false, nullptr));
    THROW_LAST_ERROR_IF_NULL(m_feedEventTimer);

    m_animalHealthThread.reset(CreateThread(nullptr, 0, AnimalHealth, this, 0, nullptr));
    m_animalInteractivityThread.reset(CreateThread(nullptr, 0, AnimalInteractivity, this, 0, nullptr));

    HealthEvent.create(wil::EventOptions::Signaled);
    THROW_LAST_ERROR_IF(!HealthEvent.is_valid());

    m_feedDueTime.QuadPart = -((GameManager::FeedEventMinutes * 60) * TIMER_SECONDS);

    ResetFeedTimer(m_feedEventTimer.get());
}

bool Cage::IsCageEmpty() const noexcept {
    auto guard = CriticalSection.lock();

    return Animals.empty();
}

DWORD Cage::GetAverageInteractiveLevel() const noexcept {
    DWORD total = 0;
    DWORD count = 0;

    auto guard = CriticalSection.lock();

    if (IsCageEmpty()) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return total;
    }

    auto animalGuard = Animal::CriticalSection.lock();

    for (const auto& animal : Animals) {
        count++;
        total += animal->InteractiveLevel;
    }

    return total / count;
}

DWORD Cage::GetTotalInteractiveLevel() const noexcept {
    DWORD total = 0;

    auto guard = CriticalSection.lock();

    if (IsCageEmpty()) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return total;
    }

    auto animalGuard = Animal::CriticalSection.lock();

    for (const auto& animal : Animals) {
        total += animal->InteractiveLevel;
    }

    return total;
}

// Adds an animal to the cage list
void Cage::AddAnimal(wistd::unique_ptr<Animal> animal) {
    auto guard = CriticalSection.lock();

    Animals.push_back(move(animal));
}

// Completely removes an animal from the cage and zoo
void Cage::RemoveAnimal(LPCTSTR uniqueName) {
    auto guard = CriticalSection.lock();

    Helpers::AddRandomName(uniqueName);

    Animals.erase(
        std::remove_if(
            Animals.begin(),
            Animals.end(),
            [&](wistd::unique_ptr<Animal>& cagedAnimal) { return cagedAnimal->UniqueName == uniqueName; }
        ),
        Animals.end()
    );
}

// Waits for Animal Health and Interactivity threads to close
void Cage::WaitForThreads() const noexcept {
    WaitForSingleObject(m_animalHealthThread.get(), INFINITE);
    WaitForSingleObject(m_animalInteractivityThread.get(), INFINITE);
}

// Converts a number to "Cage#" string
LPCTSTR Cage::GetCageName(const int number) {
    /*_tstringstream cageName;
    cageName << _T("Cage") << number;

    return cageName.str().c_str();*/
    switch (number) {
        case 1 :
            return _T("Cage1");
        case 2 :
            return _T("Cage2");
        case 3 :
            return _T("Cage3");
        case 4 :
            return _T("Cage4");
        case 5 :
            return _T("Cage5");
        case 6 :
            return _T("Cage6");
        case 7 :
            return _T("Cage7");
        case 8 :
            return _T("Cage8");
        case 9 :
            return _T("Cage9");
        case 10 :
            return _T("Cage10");
        case 11 :
            return _T("Cage11");
        case 12 :
            return _T("Cage12");
        default :
            return _T("Invalid Cage");
    }
}

void Cage::ResetFeedTimer(HANDLE feedEventTimer) {
    THROW_LAST_ERROR_IF(
        !SetWaitableTimer(feedEventTimer, &m_feedDueTime, 0, NULL, NULL, FALSE)
    );
}

DWORD WINAPI Cage::AnimalHealth(LPVOID lpParam) {
    const auto cage = static_cast<Cage*>(lpParam);

    HANDLE events[3];

    events[0] = cage->FeedEvent.get();
    events[1] = cage->m_feedEventTimer.get();
    events[2] = GameManager::AppClose.get();

    do {
        const auto waitResult = WaitForMultipleObjects(_countof(events), events, false, INFINITE);

        if (waitResult == 2) {
            return 0;
        }

        if (cage->IsCageEmpty()) { continue; }

        auto guard = CriticalSection.lock();

        for (auto i = cage->Animals.size(); i-- > 0;) {
            try {
                if (waitResult == 0) {
                    cage->Animals.at(i)->AddHealthLevel();
                } else if (waitResult == 1) {
                    cage->Animals.at(i)->RemoveHealthLevel();
                }

                cage->Animals.at(i)->SetHealthEvent();
            } catch (...) {
                cwl::WriteLine(_T("Unable to set HealthEvent: %d"), GetLastError());
            }
        }

        ResetFeedTimer(cage->m_feedEventTimer.get());
    } while (true);
}

DWORD WINAPI Cage::AnimalInteractivity(LPVOID lpParam) {
    const auto cage = static_cast<Cage*>(lpParam);

    HANDLE events[2];

    events[0] = cage->HealthEvent.get();
    events[1] = GameManager::AppClose.get();

    do {
        if (WaitForMultipleObjects(_countof(events), events, false, INFINITE) == 1) {
            return 0;
        }

        if (cage->IsCageEmpty()) { continue; }

        auto guard = CriticalSection.lock();

        for (auto const& animal : cage->Animals) {
            auto animalGuard = Animal::CriticalSection.lock();

            if (animal->HealthLevelChange) {

                if (animal->InteractivityPrompted) {
                    animal->AddInteractiveLevel();
                } else {
                    animal->RemoveInteractiveLevel();
                }

                animal->HealthLevelChange = false;
            }
        }
    } while (true);
}
