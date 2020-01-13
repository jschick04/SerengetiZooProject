#include "SignificantEvent.h"
#include <ConsoleColors.h>
#include <ctime>
#include <cwl.h>
#include <memory>
#include <tchar.h>
#include <wil/resource.h>
#include "GameManager.h"
#include "Helpers.h"
#include "Zoo.h"

LARGE_INTEGER SignificantEvent::DueTime{DueTime.QuadPart = 0};

SignificantEvent::SignificantEvent(std::vector<Cage*> cages) {
    m_timer.reset(CreateWaitableTimer(nullptr, false, nullptr));
    THROW_LAST_ERROR_IF(!m_timer.is_valid());

    auto params = wil::make_unique_failfast<EventParams>();

    params->Cages = move(cages);
    params->Timer = m_timer.get();

    m_significantEventThread.reset(CreateThread(nullptr, 0, SignificantEventTimer, params.release(), 0, nullptr));

    DueTime.QuadPart = -((GameManager::SignificantEventMinutes * 60) * TIMER_SECONDS);
    m_feedDueTime.QuadPart = -((GameManager::FeedEventMinutes * 60) * TIMER_SECONDS);

    THROW_LAST_ERROR_IF(
        !SetWaitableTimer(m_timer.get(), &DueTime, 0, nullptr, nullptr, false)
    );
}

void SignificantEvent::WaitForThread() const noexcept {
    WaitForSingleObject(m_significantEventThread.get(), INFINITE);
}

DWORD WINAPI SignificantEvent::SignificantEventTimer(LPVOID lpParam) {
    const auto params = static_cast<EventParams*>(lpParam);
    const auto param = wil::make_unique_failfast<EventParams>(*params);

    srand(unsigned(time(nullptr)) * GetProcessId(GetCurrentProcess()));

    HANDLE events[2];

    events[0] = params->Timer;
    events[1] = GameManager::AppClose.get();

    do {
        if (WaitForMultipleObjects(2, events, false, INFINITE) == 1) {
            return 0;
        }

        BOOL action;
        Animal* selectedAnimal = nullptr;

        auto guard = Cage::CriticalSection.lock();

        int cageCount = 0;

        for (auto cage : params->Cages) {
            if (!cage->IsCageEmpty()) {
                cageCount++;
            }
        }

        if (cageCount <= 0) { continue; }

        Cage* randomCage;

        do {
            randomCage = params->Cages.at(rand() % params->Cages.size());
        } while (randomCage->IsCageEmpty());

        while (selectedAnimal == nullptr) {

            for (auto const& animal : randomCage->Animals) {
                action = rand() % 2;

                if (action) {
                    selectedAnimal = animal.get();
                    break;
                }
            }
        };

        action = rand() % 2;

        if (action) {
            cwl::WriteLine(
                _T("\n%c%s the %s has escaped!\n\n"),
                YELLOW,
                selectedAnimal->UniqueName,
                Helpers::AnimalTypeToString(selectedAnimal->AnimalType)
            );
            cwl::WriteLine(
                _T("You have %clost%r %d points because all visitors left the zoo...\n"),
                PINK,
                Visitor::GetVisitorCount()
            );
            GameManager::Score -= Visitor::GetVisitorCount();

            randomCage->RemoveAnimal(selectedAnimal->UniqueName);
        } else {
            cwl::WriteLine(
                _T("\n%c%s the %s has given birth to a baby %s!\n\n"),
                LIME,
                selectedAnimal->UniqueName,
                Helpers::AnimalTypeToString(selectedAnimal->AnimalType),
                Helpers::AnimalTypeToString(selectedAnimal->AnimalType)
            );
            cwl::WriteLine(
                _T("All visitors have left for the day and you %cearned%r %d points...\n"),
                LIME,
                3 * Visitor::GetVisitorCount()
            );
            GameManager::Score += 3 * Visitor::GetVisitorCount();

            auto newAnimal = wil::make_unique_failfast<Animal>(
                selectedAnimal->AnimalType,
                Helpers::GetRandomName(),
                randomCage->Name
            );

            randomCage->AddAnimal(move(newAnimal));
        }

        THROW_LAST_ERROR_IF(
            !SetWaitableTimer(params->Timer, &DueTime, 0, nullptr, nullptr, false)
        );

        Zoo::EndTurn();
    } while (TRUE);
}
