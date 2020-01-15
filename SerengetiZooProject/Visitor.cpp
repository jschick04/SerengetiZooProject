#include "Visitor.h"

#include <ConsoleColors.h>
#include <cwl.h>
#include <random>
#include <tchar.h>
#include "Cage.h"
#include "GameManager.h"
#include "Helpers.h"

wil::critical_section Visitor::CriticalSection;

Visitor::Visitor(std::vector<Cage*> cages) : m_movementTime() {
    UniqueName = Helpers::GetRandomName();
    CageLocation = _T("Entry");
    HappinessLevel = 8;
    Status = VisitorStatus::Happy;

    m_cages = move(cages);
    m_currentCageNumber = -1;

    m_movementTimer.reset(CreateWaitableTimer(nullptr, false, nullptr));
    THROW_LAST_ERROR_IF(!m_movementTimer.is_valid());

    m_visitorLoopThread.reset(CreateThread(nullptr, 0, VisitorLoop, this, 0, nullptr));

    ResetMovementTimer();

    cwl::WriteLine(_T("%c%s has entered the Zoo\n"), LIME, UniqueName);
}

void Visitor::WaitForThreads() const noexcept {
    WaitForSingleObject(m_visitorLoopThread.get(), INFINITE);
}

// Updates global score based on happiness level
void Visitor::CalculateScore(Visitor* visitor) noexcept {
    if (visitor->HappinessLevel < 5) {
        visitor->Status = VisitorStatus::RefundDemanded;
        cwl::WriteLine(_T("%c%s is %s\n"), PINK, visitor->UniqueName, Helpers::VisitorStatusToString(visitor->Status));
        GameManager::Score -= 3;
    } else if (visitor->HappinessLevel <= 7) {
        visitor->Status = VisitorStatus::LeavingAngry;
        cwl::WriteLine(
            _T("%c%s is %s\n"),
            YELLOW,
            visitor->UniqueName,
            Helpers::VisitorStatusToString(visitor->Status)
        );
        GameManager::Score -= 1;
    } else {
        visitor->Status = VisitorStatus::LeavingHappy;
        cwl::WriteLine(_T("%c%s is %s\n"), LIME, visitor->UniqueName, Helpers::VisitorStatusToString(visitor->Status));
        GameManager::Score += 1;
    }
}

// Resets the movement timer to a random time
void Visitor::ResetMovementTimer() {
    std::random_device generator;
    const std::uniform_int_distribution<int> range(1, 3);

    m_movementTime.QuadPart = -((range(generator) * 10) * TIMER_SECONDS);

    THROW_LAST_ERROR_IF(
        !SetWaitableTimer(m_movementTimer.get(), &m_movementTime, 0, nullptr, nullptr, false)
    );
}

// Updates status based on the happiness level
void Visitor::UpdateStatus(Visitor* visitor) noexcept {
    if (visitor->HappinessLevel < 5) {
        visitor->Status = VisitorStatus::RefundDemanded;
    } else if (visitor->HappinessLevel <= 7) {
        visitor->Status = VisitorStatus::Disappointed;

    } else {
        visitor->Status = VisitorStatus::Happy;
    }
}

DWORD WINAPI Visitor::VisitorLoop(LPVOID lpParam) {
    const auto visitor = static_cast<Visitor*>(lpParam);

    HANDLE events[3];

    events[0] = visitor->m_movementTimer.get();
    events[1] = GameManager::CloseZoo.get();
    events[2] = GameManager::AppClose.get();

    do {
        const auto wait = WaitForMultipleObjects(_countof(events), events, false, INFINITE);

        if (wait == 1 || wait == 2) {
            auto guard = CriticalSection.lock();

            CalculateScore(visitor);

            return 0;
        }

        auto guard = CriticalSection.lock();

        visitor->m_currentCageNumber++;

        if (visitor->m_currentCageNumber >= static_cast<int>(visitor->m_cages.size()) ||
            visitor->Status == VisitorStatus::RefundDemanded) {
            CalculateScore(visitor);

            return 0;
        }

        auto currentCage = visitor->m_cages.at(visitor->m_currentCageNumber);

        visitor->CageLocation = currentCage->Name;

        cwl::WriteLine(
            _T("%s is interacting with the %s cage\n"),
            visitor->UniqueName,
            Helpers::AnimalTypeToString(currentCage->Animals.at(0)->AnimalType)
        );

        if (currentCage->IsCageEmpty()) {
            cwl::WriteLine(_T("%c%s lost 2 happiness points due to an empty cage\n"), PINK, visitor->UniqueName);
            visitor->HappinessLevel -= 2;
            continue;
        }

        int averageInteractivityLevel = currentCage->GetAverageInteractiveLevel();

        if (averageInteractivityLevel <= 4) {
            if (visitor->HappinessLevel > 0) {
                visitor->HappinessLevel -= 1;
                cwl::WriteLine(
                    _T("%s is %cDisappointed%r with the %s cage!\n"),
                    visitor->UniqueName,
                    RED,
                    Helpers::AnimalTypeToString(currentCage->Animals.at(0)->AnimalType)
                );
            }
        } else {
            if (visitor->HappinessLevel > 5 && visitor->HappinessLevel < 10) {
                visitor->HappinessLevel += 1;
            }
        }

        UpdateStatus(visitor);

        visitor->ResetMovementTimer();
    } while (true);
}
