#include "SignificantEvent.h"
#include <ctime>
#include <cwl.h>
#include <memory>
#include <tchar.h>
#include <wil/resource.h>
#include "GameManager.h"

SignificantEvent::SignificantEvent(std::vector<Cage*> cages) {
    m_timer.reset(CreateWaitableTimer(nullptr, false, nullptr));
    THROW_LAST_ERROR_IF(!m_timer.is_valid());
    
    auto params = wil::make_unique_failfast<EventParams>();

    params->Cages = move(cages);
    params->Timer = m_timer.get();
    
    m_significantEventThread.reset(CreateThread(nullptr, 0, SignificantEventTimer, params.release(), 0, nullptr));

    m_dueTime.QuadPart = -((GameManager::SignificantEventMinutes * 60) * TIMER_SECONDS);
    m_feedDueTime.QuadPart = -((GameManager::FeedEventMinutes * 60) * TIMER_SECONDS);

    THROW_LAST_ERROR_IF(
        !SetWaitableTimer(m_timer.get(), &m_dueTime, 0, nullptr, nullptr, false)
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
        //Animal* selectedAnimal = nullptr;

        /*EnterCriticalSection(&AnimalListCrit);

        Cage* randomCage;

        do {
            randomCage = GetRandomCage();
        } while (randomCage == NULL);

        NodeEntry* temp = animalListHead->Blink;

        while (selectedAnimal == NULL) {
            if (temp == animalListHead) { temp = temp->Blink; }

            ZooAnimal* tempAnimal = &CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

            if (_tcscmp(tempAnimal->CageName, randomCage->Name) == 0) {
                action = rand() % 2;

                if (action) {
                    selectedAnimal = tempAnimal;
                    break;
                }
            }

            temp = temp->Blink;
        };

        action = rand() % 2;

        if (selectedAnimal != NULL) {
            if (action) {
                cwl::WriteLine(
                    _T("\n%c%s the %s has escaped!\n\n"),
                    YELLOW,
                    selectedAnimal->UniqueName,
                    AnimalTypeToString(selectedAnimal->AnimalType)
                );
                cwl::WriteLine(
                    _T("You have %clost%r %d points because all visitors left the zoo...\n"),
                    PINK,
                    GetVisitorCount(visitorListHead)
                );
                g_Score -= GetVisitorCount(visitorListHead);

                RemoveAnimal(selectedAnimal);
            } else {
                cwl::WriteLine(
                    _T("\n%c%s the %s has given birth to a baby %s!\n\n"),
                    LIME,
                    selectedAnimal->UniqueName,
                    AnimalTypeToString(selectedAnimal->AnimalType),
                    AnimalTypeToString(selectedAnimal->AnimalType)
                );
                cwl::WriteLine(
                    _T("All visitors have left for the day and you %cearned%r %d points...\n"),
                    LIME,
                    3 * GetVisitorCount(visitorListHead)
                );
                g_Score += 3 * (int)GetVisitorCount(visitorListHead);

                NewAnimal(
                    selectedAnimal->AnimalType,
                    GetRandomName(),
                    selectedAnimal->CageName
                );
            }
        }

        LeaveCriticalSection(&AnimalListCrit);

        if (!SetWaitableTimer(significantEventTimer, &seDueTime, 0, NULL, NULL, FALSE)) {
            cwl::WriteLine(_T("Failed to set Significant Event Timer: %d\n"), GetLastError());
        }

        EndTurnActions();*/
    } while (TRUE);
}
