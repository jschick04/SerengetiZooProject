#include "SerengetiZooProject.h"
#include "Animals.h"
#include "Visitor.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <WriteLine.h>
#include <ConsoleColors.h>

#define MAXS 20
#define MAXA 5

NodeEntry* animalListHead = 0;
NodeEntry* visitorListHead = 0;

int mTurns = 15;
HANDLE hTimer = NULL;
LARGE_INTEGER liDueTime;
HANDLE tEvent;
int tThread = 0;

DWORD WINAPI mTimer(LPVOID lpParam);
void printScore();

//void printvHappinessLevel();
//void NextTurn();

BOOL InitializeListHeads() {
    animalListHead = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    visitorListHead = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));

    if (animalListHead == 0 || visitorListHead == 0) {
        return FALSE;
    }

    animalListHead->Flink = animalListHead->Blink = animalListHead;
    visitorListHead->Flink = visitorListHead->Blink = visitorListHead;

    return TRUE;
}

void InitializeMain() {
    appClose = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (appClose == NULL) {
        ConsoleWriteLine(_T("%cFailed to create critical event: %d"), RED, GetLastError());
        ExitProcess(1);
    }

    if (InitializeListHeads() == FALSE) {
        ConsoleWriteLine(_T("%cFailed to create list heads\n"), RED);
        ExitProcess(1);
    }

    if (!InitializeCriticalSectionAndSpinCount(&AnimalListCrit, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Animal List CRITICAL_SECTION"), RED);
    }

    if (!InitializeCriticalSectionAndSpinCount(&ConsoleCrit, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Console CRITICAL_SECTION"), RED);
    }

    if (!InitializeCriticalSectionAndSpinCount(&cScore, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Score CRITICAL_SECTION"), RED);
    }
}

void InitializeZoo() {
    g_Score = 0;

    // Initialize animals structs
    LPTSTR uniqueName[] = {
        _T("Julien"),
        _T("Melman"),
        _T("Maurice"),
        _T("Gloria"),
        _T("Mason"),
    };

    for (int i = 0; i != _countof(cages); ++i) {
        const DWORD interactiveLevel = (rand() % (6 - 4 + 1)) + 4;

        const LPTSTR cageName = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TCHAR) * 10);
        const LPTSTR prepend = _T("Cage");

        if (cageName == NULL) {
            continue; // TODO: Add better handling if we can't assign a name
        }

        StringCchPrintf(cageName, 10, _T("%s%d"), prepend, i + 1);

        cages[i] = NewCage(cageName);

        NewAnimal(i, uniqueName[i], cageName, interactiveLevel);

        // Breaks cage name but need to cleanup at some point
        //HeapFree(GetProcessHeap(), 0, cageName);
    }

    // Initialize visitor structs
    AddVisitor(visitorListHead, _T("Tom"));
    AddVisitor(visitorListHead, _T("Jerry"));
    AddVisitor(visitorListHead, _T("Cornelius"));
    AddVisitor(visitorListHead, _T("Fred"));
    EnumVisitors(visitorListHead, TRUE);

    //BareBones Entry to test the random visitor entering simulation test.
    CreateThread(
        NULL,
        0,
        AddVisitorsThread,
        NULL,
        0,
        NULL
    );
}

void EndTurnActions() {
    DecreaseAnimalFedTimer();
}

void Dispose() {
    SetEvent(appClose);

    CancelWaitableTimer(hTimer);
    //if(ht)TerminateThread(ht,0);
    tThread = 1;

    for (int i = 0; i != _countof(cages); ++i) {
        WaitForSingleObject(cages[i]->AnimalHealthThread, INFINITE);
    }

    HeapFree(GetProcessHeap(), 0, animalListHead);
    HeapFree(GetProcessHeap(), 0, visitorListHead);
    // TODO: Need to terminate Visitor threads or this will throw an exception

    ExitProcess(0);
}

int _tmain() {
    InitializeMain();

    TCHAR buffer[MAX_PATH];
    int menuOption;
    int cageNumber;

    InitVisitorsEvent();
    InitializeZoo(); // TODO: Need to error handle

    DWORD tid = 0;
    const HANDLE ht = CreateThread(NULL, 0, mTimer, 0, 0, &tid);
    if (ht == NULL) {
        ConsoleWriteLine(_T("%cError creating timer thread: %d\n"), RED, GetLastError());
    }
GAMELOOP:
    LeaveCriticalSection(&ConsoleCrit);

    ConsoleWriteLine(_T("Please select your action\n"));
    ConsoleWriteLine(_T("-------------------------\n"));
    ConsoleWriteLine(_T("1 - Feed Animals\n"));
    ConsoleWriteLine(_T("2 - Check Animal Interactivity Levels\n"));
    ConsoleWriteLine(_T("3 - Display Current Disposition of visitors\n"));
    ConsoleWriteLine(_T("4 - Show Case Animal\n"));
    ConsoleWriteLine(_T("5 - Check Visitors Happiness Level\n"));
    ConsoleWriteLine(_T("6 - Turn\n"));
    ConsoleWriteLine(_T("0 - Exit\n"));

    LeaveCriticalSection(&ConsoleCrit);

    _fgetts(buffer, _countof(buffer), stdin);
    if (_stscanf_s(buffer, _T("%d"), &menuOption) != 1) {
        ConsoleWriteLine(_T("Invalid Selection...\n"));
        goto GAMELOOP;
    }

    switch (menuOption) {
        case 1 : // Feed Animal
            /*Call a function from Animal.c that prints all the animals within the list and their respective health level.
            Call a function from Animal.c that triggers the feeding of a particular animal within the list. This functions should accept a string that would be compared to Animal->UniqueName
            In order to be compliant with the code requirements, this function should also check if the feedevent has been set, I will set the event and then call the feed function passing the animal name as a string.
            */
            EnterCriticalSection(&ConsoleCrit);

            ConsoleWriteLine(_T("%cYou selected - Feed Animals\n"),BLUE);

            GetAllAnimalsHealth();

            ConsoleWriteLine(_T("Which cage number would you like to feed?\n"));
            _fgetts(buffer, _countof(buffer), stdin);
            if (_stscanf_s(buffer, _T("%d"), &cageNumber) == 1) {
                if (cageNumber < 1 || cageNumber > (int)_countof(cages)) {
                    ConsoleWriteLine(_T("Invalid Selection...\n"));
                    goto GAMELOOP;
                }

                SetEvent(cages[cageNumber - 1]->FeedEvent);
            }

            LeaveCriticalSection(&ConsoleCrit);

            break;
        case 2 : // Check Animal Interactivity Levels
            /*Call a function from Animal.c that prints all the animals within the list and their respective Interactivity Levels.
            */
            EnterCriticalSection(&ConsoleCrit);

            ConsoleWriteLine(_T("%cYou selected - Check Animal Int Levels\n"),BLUE);
            GetAllAnimalsInteractivity();

            LeaveCriticalSection(&ConsoleCrit);
            break;
        case 3 : // Display Current Disposition of visitors
            /*Call a function from Vistor.c that prints all the visitors within the list and their respective CageLocation.
            */
            ConsoleWriteLine(_T("%cYou selected - Display Current Disp of Visitors\n"),BLUE);
            break;
        case 4 : // Show Case Animal
            /*Call a function from Visitor.c that increases the HappinessLevel of all visitors that are currently at a specific cage. This functions should accept a string to be compared with each visitor Visitor->CageLocation. 
            */
            //Call NextTurn()
            ConsoleWriteLine(_T("%cYou selected - Show Case Animal\n"),BLUE);
            break;
        case 5 : // Check Visitors Happiness Level
            /*Call a function from Vistor.c that prints all the visitors within the list and their respective HappinessLevel.
            */
            ConsoleWriteLine(_T("%cYou selected - Check Visitors Happ Level\n"),BLUE);
            EnumVisitors(visitorListHead, TRUE);
            break;
        case 6 : //Next Turn
            //Calls NextTurn() function which signal Visitors and Animals that they can move one step forward.
            //Print current score and Happiness Level.
            ConsoleWriteLine(_T("%cYou selected - Next Turn\n"),BLUE);
            EndTurnActions();
            break;
        case 0 :
            ConsoleWriteLine(_T("%cYou selected - Quit\n"),BLUE);
            Dispose();
        default :
            ConsoleWriteLine(_T("Invalid Selection...\n"));
            break;
    }
    CancelWaitableTimer(hTimer);
    SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
    printScore();
    goto GAMELOOP;
}

DWORD WINAPI mTimer(LPVOID lpParam) {
    lpParam = "10";
    liDueTime.QuadPart = -100000000LL;
    // Create an unnamed waitable timer.
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (NULL == hTimer) {
        ConsoleWriteLine(_T("CreateWaitableTimer failed (%d)\n"), GetLastError());
        return 1;
    }
    // Set a timer to wait for 60 seconds.
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
        ConsoleWriteLine(_T("SetWaitableTimer failed (%d)\n"), GetLastError());
        return 2;
    }
    // Wait for the timer.
mtimerloop:
    if (tThread != 0)return 0;
    if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
        ConsoleWriteLine(_T("WaitForSingleObject failed (%d)\n"), GetLastError());
    else {
        ConsoleWriteLine(_T("\n%c------------------------------------\n"),RED);
        ConsoleWriteLine(_T("%cYou took too long to select your option\nPlease select an option from the menu.\n"),RED);

    }
    SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
    goto mtimerloop;

}

void printScore() {
    ConsoleWriteLine(_T("%c-------------------------\n"),YELLOW);
    ConsoleWriteLine(_T("%c Score = %d\n"),YELLOW, g_Score);
    ConsoleWriteLine(_T("%c-------------------------\n"),YELLOW);
}
