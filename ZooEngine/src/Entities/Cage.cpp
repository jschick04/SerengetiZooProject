#include "zepch.h"

#include "Cage.h"

#include <algorithm>

#include "Game.h"

namespace SerengetiZoo
{
    Cage::Cage(const int number)
    {
        m_name = GetCageName(number);
        m_cageType = Animal::GetRandomAnimalType();
        AddAnimal();

        m_feedEvent.create(wil::EventOptions::None);
        THROW_LAST_ERROR_IF(!m_feedEvent.is_valid());

        m_feedEventTimer.reset(CreateWaitableTimer(nullptr, false, nullptr));
        THROW_LAST_ERROR_IF_NULL(m_feedEventTimer);

        m_animalHealthThread.reset(CreateThread(nullptr, 0, AnimalHealth, this, 0, nullptr));
        m_animalInteractivityThread.reset(CreateThread(nullptr, 0, AnimalInteractivity, this, 0, nullptr));

        m_healthEvent.create(wil::EventOptions::Signaled);
        THROW_LAST_ERROR_IF(!m_healthEvent.is_valid());

        m_feedDueTime.QuadPart = -((GameSettings::FeedEventMinutes * 60) * TimerSeconds);

        ResetFeedTimer(m_feedEventTimer.get());
    }

    bool Cage::IsCageEmpty()
    {
        auto guard = m_cs.lock();

        return m_animals.empty();
    }

    DWORD Cage::GetAverageInteractiveLevel()
    {
        DWORD total = 0;
        DWORD count = 0;

        auto guard = m_cs.lock();

        if (IsCageEmpty())
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);

            return total;
        }

        for (const auto& animal : m_animals)
        {
            count++;
            total += animal.GetInteractiveLevel();
        }

        return total / count;
    }

    DWORD Cage::GetTotalInteractiveLevel()
    {
        DWORD total = 0;

        auto guard = m_cs.lock();

        if (IsCageEmpty())
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);

            return total;
        }

        for (const auto& animal : m_animals)
        {
            total += animal.GetInteractiveLevel();
        }

        return total;
    }

    // Adds an animal to the cage list
    void Cage::AddAnimal()
    {
        auto guard = m_cs.lock();

        m_animals.emplace_back(m_cageType);
    }

    void Cage::FeedAnimals() const
    {
        m_feedEvent.SetEvent();

        auto lock = Renderer::GetConsoleLock().lock();

        cwl::WriteLine(_T("%cAnimals in %s have been fed\n"), LIME, m_name);
    }

    void Cage::GetAnimals()
    {
        auto guard = m_cs.lock();

        for (auto const& animal : m_animals)
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("[%c%s%r] %s the %s\n"),SKYBLUE, m_name, animal.GetName(), animal.GetType());
        }
    }

    void Cage::GetAnimalsHealth()
    {
        auto guard = m_cs.lock();

        for (auto const& animal : m_animals)
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(
                _T("[%c%s%r] %s the %s "),
                SKYBLUE,
                m_name,
                animal.GetName(),
                animal.GetType());

            if (animal.GetHealthLevel() >= 6)
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), LIME, animal.GetHealthLevel());
            }
            else if (animal.GetHealthLevel() >= 3)
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, animal.GetHealthLevel());
            }
            else
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), PINK, animal.GetHealthLevel());
            }
        }
    }

    void Cage::GetAnimalsInteractiveLevel()
    {
        auto guard = m_cs.lock();

        for (auto const& animal : m_animals)
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(
                _T("[%c%s%r] %s the %s "),
                SKYBLUE,
                m_name,
                animal.GetName(),
                animal.GetType());

            if (animal.GetInteractiveLevel() >= 6)
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), LIME, animal.GetInteractiveLevel());
            }
            else if (animal.GetInteractiveLevel() >= 3)
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), YELLOW, animal.GetInteractiveLevel());
            }
            else
            {
                cwl::WriteLine(_T("(%c%d%r)\n"), PINK, animal.GetInteractiveLevel());
            }
        }
    }

    Animal* Cage::GetRandomAnimal()
    {
        auto guard = m_cs.lock();

        if (IsCageEmpty()) { return nullptr; }

        std::random_device rd;
        std::mt19937 generator(rd());

        return &m_animals.at(generator() % m_animals.size());
    }

    // Completely removes an animal from the cage and zoo
    void Cage::RemoveAnimal(const Animal& animal)
    {
        auto guard = m_cs.lock();

        const auto iterator = std::ranges::find(m_animals, animal);

        if (iterator == m_animals.end()) { return; }

        Helpers::AddRandomName(animal.GetName());
        m_animals.erase(iterator);
    }

    // Waits for Animal Health and Interactivity threads to close
    void Cage::WaitForThreads() const noexcept
    {
        WaitForSingleObject(m_animalHealthThread.get(), INFINITE);
        WaitForSingleObject(m_animalInteractivityThread.get(), INFINITE);
    }

    // Converts a number to "Cage#" string
    LPCTSTR Cage::GetCageName(const int number)
    {
        /*_tstringstream cageName;
        cageName << _T("Cage") << number;

        return cageName.str().c_str();*/
        switch (number)
        {
            case 1:
                return _T("Cage1");
            case 2:
                return _T("Cage2");
            case 3:
                return _T("Cage3");
            case 4:
                return _T("Cage4");
            case 5:
                return _T("Cage5");
            case 6:
                return _T("Cage6");
            case 7:
                return _T("Cage7");
            case 8:
                return _T("Cage8");
            case 9:
                return _T("Cage9");
            case 10:
                return _T("Cage10");
            case 11:
                return _T("Cage11");
            case 12:
                return _T("Cage12");
            default:
                return _T("Invalid Cage");
        }
    }

    void Cage::ResetFeedTimer(const HANDLE feedEventTimer) const
    {
        THROW_LAST_ERROR_IF(!SetWaitableTimer(feedEventTimer, &m_feedDueTime, 0, NULL, NULL, FALSE));
    }

    DWORD WINAPI Cage::AnimalHealth(const LPVOID lpParam)
    {
        const auto cage = static_cast<Cage*>(lpParam);

        HANDLE events[3];

        events[0] = cage->m_feedEvent.get();
        events[1] = cage->m_feedEventTimer.get();
        events[2] = Game::EndGame.get();

        do
        {
            const auto waitResult = WaitForMultipleObjects(_countof(events), events, false, INFINITE);

            if (waitResult == 2)
            {
                return 0;
            }

            if (cage->IsCageEmpty()) { continue; }

            auto guard = cage->m_cs.lock();

            for (auto i = cage->m_animals.size(); i-- > 0;)
            {
                auto& animal = cage->m_animals.at(i);

                if (waitResult == 0)
                {
                    animal.AddHealthLevel();
                }
                else if (waitResult == 1)
                {
                    animal.RemoveHealthLevel();
                }

                if (animal.GetHealthLevel() >= 1 && animal.GetHealthLevel() < 5)
                {
                    auto lock = Renderer::GetConsoleLock().lock();

                    cwl::WriteLine(_T("%s the %s is %csick\n"), animal.GetName(), animal.GetType(), PINK);

                    cage->m_healthEvent.SetEvent();
                }

                if (animal.GetHealthLevel() < 1)
                {
                    auto lock = Renderer::GetConsoleLock().lock();

                    cwl::WriteLine(
                        _T(
                            "\n%c%s the %s is seriously ill and the Zoo Oversight Committee has relocated the animal\n\n"),
                        PINK,
                        animal.GetName(),
                        animal.GetType());

                    //GameManager::Score -= 3;

                    cage->RemoveAnimal(animal);
                }
            }

            cage->ResetFeedTimer(cage->m_feedEventTimer.get());
        }
        while (true);
    }

    DWORD WINAPI Cage::AnimalInteractivity(const LPVOID lpParam)
    {
        const auto cage = static_cast<Cage*>(lpParam);

        HANDLE events[2] { };

        events[0] = cage->m_healthEvent.get();
        events[1] = Game::EndGame.get();

        do
        {
            if (WaitForMultipleObjects(_countof(events), events, false, INFINITE) == 1)
            {
                return 0;
            }

            if (cage->IsCageEmpty()) { continue; }

            auto guard = cage->m_cs.lock();

            for (auto& animal : cage->m_animals)
            {
                if (animal.GetHasHealthChanged())
                {
                    if (animal.GetIsInteractivityPrompted())
                    {
                        animal.AddInteractiveLevel();
                    }
                    else
                    {
                        animal.RemoveInteractiveLevel();
                    }
                }
            }
        }
        while (true);
    }
}
