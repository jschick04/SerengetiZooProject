#include "SignificantEvent.h"

DWORD WINAPI SignificantEvent::SignificantEventTimer(LPVOID) {
    /*srand((unsigned)time(NULL) * GetProcessId(GetCurrentProcess()));

    HANDLE events[2];

    events[0] = significantEventTimer;
    events[1] = appClose;

    do {
        if (WaitForMultipleObjects(2, events, FALSE, INFINITE) == 1) {
            return 0;
        }

        BOOL action;
        ZooAnimal* selectedAnimal = NULL;

        if (IS_LIST_EMPTY(animalListHead)) { continue; }

        EnterCriticalSection(&AnimalListCrit);

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

        EndTurnActions();
    } while (TRUE);*/

    return 0;
}