#include "Zoo.h"
#include <ConsoleColors.h>
#include <cwl.h>
#include <tchar.h>
#include "Menu.h"

Zoo::Zoo(const int numberOfCages) {
    Score = 0;

    for (int i = 1; i < numberOfCages + 1; i++) {
        auto cageName = wil::str_concat<LPCTSTR>(_T("Cage"), i);
        Cages.push_back(wil::make_unique_failfast<Cage>(cageName));
    }
}

void Zoo::EndTurn() {
    cwl::WriteLine(_T("\n%cZoo is closing for the rest of the day...\n"), PINK);

    /*ExitZoo();
    int ThreadHandleCount = VisitorTID;
    DWORD ret = WaitForMultipleObjects(ThreadHandleCount, hThreadHandles, TRUE, INFINITE);
    if (ret == 0xFFFFFFFF) {
        DWORD returnstring = GetLastError();
        cwl::WriteLine(_T("Visitor Wait failed with %d"), returnstring);
    }*/
    IsOpen = false;

    //ResetZooClosedTimer();

    Menu::PrintScore();
    Menu::PrintCurrentZooStatus();
    Menu::PrintOptions();
}

void Zoo::GetAllAnimals() {
    if (IS_LIST_EMPTY(animalListHead)) {
        cwl::WriteLine(_T("%cNo Animals In The Cage!\n"), PINK);
        return;
    }

    EnterCriticalSection(&AnimalListCrit);

    NodeEntry* temp = animalListHead->Blink;

    while (temp != animalListHead) {
        const ZooAnimal tempAnimal = CONTAINING_RECORD(temp, AnimalList, LinkedList)->ZooAnimal;

        cwl::WriteLine(
            _T("[%c%s%r] %s the %s\n"),
            SKYBLUE,
            tempAnimal.CageName,
            tempAnimal.UniqueName,
            AnimalTypeToString(tempAnimal.AnimalType)
        );

        temp = temp->Blink;
    };

    LeaveCriticalSection(&AnimalListCrit);
}
