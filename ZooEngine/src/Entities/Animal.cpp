#include "zepch.h"

#include "Animal.h"

namespace SerengetiZoo
{
    Animal::Animal(const AnimalType animalType)
    {
        m_name = Helpers::GetRandomName();
        m_health = GetRandomHealthLevel();
        m_interactiveLevel = GetRandomInteractiveLevel();
        m_animalType = animalType;
    }

    void Animal::AddHealthLevel()
    {
        if (m_health >= 10)
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("%c%s the %s is already full...\n"), LIME, m_name, ToString(m_animalType));

            return;
        }

        auto guard = m_cs.lock();

        const DWORD healthChange = rand() % (GameSettings::FeedLevelMax - 1) + GameSettings::FeedLevelMin;
        const auto newValue = m_health + healthChange;

        m_health = newValue >= 10 ? 10 : newValue;

        m_hasHealthChanged = TRUE;
        m_isInteractivityPrompted = TRUE;
        
        auto lock = Renderer::GetConsoleLock().lock();

        cwl::WriteLine(_T("%c%s the %s has been fed\n"), LIME, m_name, ToString(m_animalType));
    }

    void Animal::RemoveHealthLevel()
    {
        auto guard = m_cs.lock();

        const auto newValue = m_health - GameSettings::HungerLevel;

        m_health = newValue <= 0 ? 0 : newValue;

        m_hasHealthChanged = TRUE;
        m_isInteractivityPrompted = FALSE;

        if (m_health < 5)
        {
            auto lock = Renderer::GetConsoleLock().lock();

            cwl::WriteLine(_T("%c%s the %s is hungry\n"), YELLOW, m_name, ToString(m_animalType));
        }
    }

    void Animal::AddInteractiveLevel()
    {
        if (m_interactiveLevel >= 10) { return; }

        auto guard = m_cs.lock();

        const DWORD interactiveChange = rand() % (GameSettings::FeedLevelMax - 1) + GameSettings::FeedLevelMin;
        const auto newValue = m_interactiveLevel + interactiveChange;

        m_interactiveLevel = newValue >= 10 ? 10 : newValue;
    }

    void Animal::RemoveInteractiveLevel()
    {
        if (m_interactiveLevel <= 0) { return; }

        auto guard = m_cs.lock();

        const auto newValue = m_interactiveLevel - GameSettings::HungerLevel;

        m_interactiveLevel = newValue <= 0 ? 0 : newValue;

        m_hasHealthChanged = FALSE;
        m_isInteractivityPrompted = FALSE;
    }

    AnimalType Animal::GetRandomAnimalType()
    {
        std::random_device generator;
        std::uniform_int_distribution range(0, 9);

        while (true)
        {
            auto selectedType = static_cast<AnimalType>(range(generator));

            if (s_availableTypes[selectedType])
            {
                s_availableTypes[selectedType] = false;

                return selectedType;
            }
        }
    }

    int Animal::GetRandomHealthLevel()
    {
        std::random_device generator;
        std::uniform_int_distribution range(5, 8);

        return range(generator);
    }

    int Animal::GetRandomInteractiveLevel()
    {
        std::random_device generator;
        std::uniform_int_distribution range(5, 8);

        return range(generator);
    }
}
