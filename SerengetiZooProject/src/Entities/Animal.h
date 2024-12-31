#pragma once

#include <map>

namespace SerengetiZoo
{
    class Cage;

    enum class AnimalType : uint8_t
    {
        Antelope, Cheetah, Giraffe, Hyaena, Hippo, Monkey, Mongoose, Tiger, WildeBeast, Zebra
    };

    inline LPCTSTR ToString(const AnimalType type)
    {
        switch (type)
        {
            case AnimalType::Antelope: return _T("Antelope");
            case AnimalType::Cheetah: return _T("Cheetah");
            case AnimalType::Giraffe: return _T("Giraffe");
            case AnimalType::Hyaena: return _T("Hyaena");
            case AnimalType::Hippo: return _T("Hippo");
            case AnimalType::Monkey: return _T("Monkey");
            case AnimalType::Mongoose: return _T("Mongoose");
            case AnimalType::Tiger: return _T("Tiger");
            case AnimalType::WildeBeast: return _T("WildeBeast");
            case AnimalType::Zebra: return _T("Zebra");
        }

        return _T("Unknown");
    }

    class Animal
    {
    public:
        static AnimalType GetRandomAnimalType();

    public:
        Animal(const Animal& other):
            m_animalType(other.m_animalType),
            m_name(other.m_name),
            m_health(other.m_health),
            m_hasHealthChanged(other.m_hasHealthChanged),
            m_interactiveLevel(other.m_interactiveLevel),
            m_isInteractivityPrompted(other.m_isInteractivityPrompted) { }

        Animal(Animal&& other) noexcept:
            m_animalType(other.m_animalType),
            m_name(other.m_name),
            m_health(other.m_health),
            m_hasHealthChanged(other.m_hasHealthChanged),
            m_interactiveLevel(other.m_interactiveLevel),
            m_isInteractivityPrompted(other.m_isInteractivityPrompted) { }

        Animal& operator=(const Animal& other)
        {
            if (this == &other) { return *this; }

            m_animalType = other.m_animalType;
            m_name = other.m_name;
            m_health = other.m_health;
            m_hasHealthChanged = other.m_hasHealthChanged;
            m_interactiveLevel = other.m_interactiveLevel;
            m_isInteractivityPrompted = other.m_isInteractivityPrompted;

            return *this;
        }

        Animal& operator=(Animal&& other) noexcept
        {
            if (this == &other) { return *this; }

            m_animalType = other.m_animalType;
            m_name = other.m_name;
            m_health = other.m_health;
            m_hasHealthChanged = other.m_hasHealthChanged;
            m_interactiveLevel = other.m_interactiveLevel;
            m_isInteractivityPrompted = other.m_isInteractivityPrompted;

            return *this;
        }

        bool operator==(const Animal& other) const
        {
            return m_name == other.m_name && m_animalType == other.m_animalType;
        }

        bool operator!=(const Animal& other) const
        {
            return !(*this == other);
        }

        ~Animal() = default;

    public:
        explicit Animal(AnimalType animalType);

        [[nodiscard]] LPCTSTR GetType() const noexcept { return ToString(m_animalType); }

        [[nodiscard]] BOOL GetHasHealthChanged() const noexcept { return m_hasHealthChanged; }

        [[nodiscard]] DWORD GetHealthLevel() const noexcept { return m_health; }

        [[nodiscard]] DWORD GetInteractiveLevel() const noexcept { return m_interactiveLevel; }

        [[nodiscard]] BOOL GetIsInteractivityPrompted() const noexcept { return m_isInteractivityPrompted; }

        [[nodiscard]] LPCTSTR GetName() const noexcept { return m_name; }

        void AddHealthLevel();
        void RemoveHealthLevel();

        void AddInteractiveLevel();
        void RemoveInteractiveLevel();

    private:
        static int GetRandomHealthLevel();
        static int GetRandomInteractiveLevel();

    private:
        static inline std::map<AnimalType, bool> s_availableTypes = {
            { AnimalType::Antelope, true },
            { AnimalType::Cheetah, true },
            { AnimalType::Giraffe, true },
            { AnimalType::Hyaena, true },
            { AnimalType::Hippo, true },
            { AnimalType::Monkey, true },
            { AnimalType::Mongoose, true },
            { AnimalType::Tiger, true },
            { AnimalType::WildeBeast, true },
            { AnimalType::Zebra, true }
        };

    private:
        AnimalType m_animalType;
        wil::critical_section m_cs;
        LPCTSTR m_name;
        DWORD m_health;
        BOOL m_hasHealthChanged = FALSE;
        DWORD m_interactiveLevel;
        BOOL m_isInteractivityPrompted = FALSE;
    };
}
