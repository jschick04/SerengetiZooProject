#include <ConsoleColors.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <tchar.h>
#include <time.h>
#include <Windows.h>
#include <WriteLine.h>
#include "Animals.h"
#include "SerengetiZooProject.h"
#include "Visitor.h"

//#define MAXS 20
//#define MAXA 5

LPTSTR uniqueNames[] = {
    _T("Julien"),
    _T("Melman"),
    _T("Maurice"),
    _T("Gloria"),
    _T("Mason"),
    _T("Ron"),
    _T("Deveroux"),
    _T("Felicio"),
    _T("Gary"),
    _T("Jorge"),
    _T("Joseph"),
    _T("Travis"),
    _T("Ryan"),
    _T("Tony"),
    _T("Leonardo"),
    _T("Barry"),
    _T("David"),
    _T("Bruce"),
    _T("Cliff"),
    _T("Jack"),
    _T("Emma"),
    _T("Taylor"),
    _T("Alex"),
    _T("Sophia"),
    _T("James"),
    _T("Diego"),
    _T("Samuel"),
    _T("Maria"),
    _T("Richard"),
    _T("Logan"),
    _T("Brianna"),
    _T("Martin"),
    _T("Jessica"),
    _T("Patricia"),
    _T("Linda"),
    _T("Caroline"),
    _T("Liam"),
    _T("Oliver"),
    _T("Ben"),
    _T("Dorothy"),
    _T("Mabel"),
    _T("Ariel"),
    _T("Louis"),
    _T("Nathan"),
    _T("Nicole"),
    _T("Adam"),
    _T("Felix"),
    _T("Jon"),
    _T("Marco"),
    _T("Harry"),
    _T("Anna"),
};

NodeEntry* animalListHead = 0;
NodeEntry* visitorListHead = 0;

HANDLE significantEventThread;

//int mTurns = 15;
//HANDLE hTimer = NULL;
//LARGE_INTEGER liDueTime;
//HANDLE tEvent;
//int tThread = 0;

//DWORD WINAPI mTimer(LPVOID lpParam);

#pragma region Helpers

int GetRandomInteractiveLevel() {
    return (rand() % 10) + 1;
}

LPTSTR GetRandomName() {
    unsigned int index;
    LPTSTR selectedName;

    EnterCriticalSection(&NameListCrit);

    do {
        index = rand() % _countof(uniqueNames);
        selectedName = uniqueNames[index];
    } while (selectedName == NULL);

    uniqueNames[index] = NULL;

    LeaveCriticalSection(&NameListCrit);

    return selectedName;
}

void AddRandomName(LPTSTR name) {
    EnterCriticalSection(&NameListCrit);

    for (int i = 0; i != _countof(uniqueNames); ++i) {
        if (uniqueNames[i] == NULL) {
            uniqueNames[i] = name;
            break;
        }
    }

    LeaveCriticalSection(&NameListCrit);
}

#pragma endregion

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
        ConsoleWriteLine(_T("%cFailed to create critical event: %d\n"), RED, GetLastError());
        ExitProcess(1);
    }

    if (InitializeListHeads() == FALSE) {
        ConsoleWriteLine(_T("%cFailed to create list heads\n"), RED);
        ExitProcess(1);
    }

    if (!InitializeCriticalSectionAndSpinCount(&AnimalListCrit, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Animal List CRITICAL_SECTION\n"), RED);
    }

    if (!InitializeCriticalSectionAndSpinCount(&NameListCrit, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Name List CRITICAL_SECTION\n"), RED);
    }

    if (!InitializeCriticalSectionAndSpinCount(&ConsoleCrit, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Console CRITICAL_SECTION\n"), RED);
    }

    if (!InitializeCriticalSectionAndSpinCount(&cScore, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Score CRITICAL_SECTION\n"), RED);
    }
}

void InitializeZoo() {
    g_Score = 0;

    for (int i = 0; i != _countof(cages); ++i) {
        const LPTSTR cageName = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TCHAR) * 10);
        const LPTSTR prepend = _T("Cage");

        if (cageName == NULL) {
            ConsoleWriteLine(_T("Failed to generate cage name: %d"), GetLastError());
            continue;
        }

        StringCchPrintf(cageName, 10, _T("%s%d"), prepend, i + 1);

        cages[i] = NewCage(cageName);

        NewAnimal(i, GetRandomName(), cageName, GetRandomInteractiveLevel());
    }

    // Initialize visitor structs
    AddVisitor(visitorListHead, _T("Tom"));
    AddVisitor(visitorListHead, _T("Jerry"));
    AddVisitor(visitorListHead, _T("Cornelius"));
    AddVisitor(visitorListHead, _T("Fred"));
    //EnumVisitors(visitorListHead, TRUE);

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

void InitializeTimers() {
    significantEventTimer = CreateWaitableTimer(NULL, FALSE, NULL);

    if (significantEventTimer == NULL) {
        ConsoleWriteLine(_T("Failed to create Significant Event Timer: %d\n"), GetLastError());
        ExitProcess(1);
    }

    significantEventThread = CreateThread(NULL, 0, SignificantEventTimer, NULL, 0, NULL);

    if (significantEventThread == NULL) {
        ConsoleWriteLine(_T("Failed to create Event Timer Threads: %d\n"), GetLastError());
        ExitProcess(1);
    }

    seDueTime.QuadPart = - ((SIGNIFICANT_EVENT_MIN * 60) * TIMER_SECONDS);
    feedDueTime.QuadPart = - ((FEED_EVENT_MIN * 60) * TIMER_SECONDS);

    if (!SetWaitableTimer(significantEventTimer, &seDueTime, 0, NULL, NULL, FALSE)) {
        ConsoleWriteLine(_T("Failed to set Significant Event Timer: %d\n"), GetLastError());
    }
}

void PrintScore() {
    ConsoleWriteLine(_T("\n%c-------------------------\n"), YELLOW);
    ConsoleWriteLine(_T("%c Score = %d\n"), YELLOW, g_Score);
    ConsoleWriteLine(_T("%c-------------------------\n"), YELLOW);
}

void EndTurnActions() {
    ConsoleWriteLine(_T("%cZoo is closing for the rest of the day...\n"), PINK);
    PrintScore();
}

void Dispose() {
    SetEvent(appClose);

    //CancelWaitableTimer(hTimer);
    ////if(ht)TerminateThread(ht,0);
    //tThread = 1;

    CancelWaitableTimer(significantEventTimer);
    WaitForSingleObject(significantEventThread, INFINITE);

    // TODO: Need to close visitor threads before cages

    for (int i = 0; i != _countof(cages); ++i) {
        CancelWaitableTimer(cages[i]->FeedEventTimer);
        WaitForSingleObject(cages[i]->AnimalHealthThread, INFINITE);
        WaitForSingleObject(cages[i]->AnimalInteractivityThread, INFINITE);
        CloseHandle(cages[i]->FeedEvent);
        HeapFree(GetProcessHeap(), 0, cages[i]->Name);
        HeapFree(GetProcessHeap(), 0, cages[i]);
    }

    HeapFree(GetProcessHeap(), 0, animalListHead);
    HeapFree(GetProcessHeap(), 0, visitorListHead);

    ExitProcess(0);
}

int _tmain() {
    srand((unsigned)time(NULL) * GetProcessId(GetCurrentProcess()));

    InitializeTimers();
    InitializeMain();

    TCHAR buffer[MAX_PATH];
    int menuOption;
    int cageNumber;

    InitVisitorsEvent();
    InitializeZoo();

    /*DWORD tid = 0;
    const HANDLE ht = CreateThread(NULL, 0, mTimer, 0, 0, &tid);
    if (ht == NULL) {
        ConsoleWriteLine(_T("%cError creating timer thread: %d\n"), RED, GetLastError());
    }*/
GAMELOOP:
    LeaveCriticalSection(&ConsoleCrit);

    ConsoleWriteLine(_T("\n%cPlease select your action\n"), LIME);
    ConsoleWriteLine(_T("%c-------------------------\n"), YELLOW);
    ConsoleWriteLine(_T("%c1%r - Feed Animals\n"), LIME);
    ConsoleWriteLine(_T("%c2%r - Check Animal Interactivity Levels\n"), LIME);
    ConsoleWriteLine(_T("%c3%r - Display Current Disposition of visitors\n"), LIME);
    ConsoleWriteLine(_T("%c4%r - Show Case Animal\n"), LIME);
    ConsoleWriteLine(_T("%c5%r - Check Visitors Happiness Level\n"), LIME);
    ConsoleWriteLine(_T("%c6%r - Close zoo for the day\n"), LIME);
    ConsoleWriteLine(_T("%c0%r - Exit\n"), PINK);
    ConsoleWriteLine(_T("%c-------------------------\n"), YELLOW);

    LeaveCriticalSection(&ConsoleCrit);

    _fgetts(buffer, _countof(buffer), stdin);
    if (_stscanf_s(buffer, _T("%d"), &menuOption) != 1) {
        ConsoleWriteLine(_T("Invalid Selection...\n"));
        goto GAMELOOP;
    }

    switch (menuOption) {
        case 1 : // Feed Animals
            EnterCriticalSection(&ConsoleCrit);

            ConsoleWriteLine(_T("\n%cYou selected - Feed Animals\n\n"), SKYBLUE);

            GetAllAnimalsHealth();

            if (IS_LIST_EMPTY(animalListHead)) {
                LeaveCriticalSection(&ConsoleCrit);
                break;
            }

            ConsoleWriteLine(_T("\nWhich cage number would you like to feed?\n"));
            _fgetts(buffer, _countof(buffer), stdin);
            if (_stscanf_s(buffer, _T("%d"), &cageNumber) == 1) {
                if (cageNumber < 1 || cageNumber > (int)_countof(cages)) {
                    ConsoleWriteLine(_T("Invalid Selection...\n"));
                } else {
                    SetEvent(cages[cageNumber - 1]->FeedEvent);
                }
            } else {
                ConsoleWriteLine(_T("Invalid Selection...\n"));
            }

            LeaveCriticalSection(&ConsoleCrit);

            break;
        case 2 : // Check Animal Interactivity Levels
            EnterCriticalSection(&ConsoleCrit);

            ConsoleWriteLine(_T("\n%cYou selected - Check Animal Int Levels\n\n"), SKYBLUE);
            GetAllAnimalsInteractivity();

            LeaveCriticalSection(&ConsoleCrit);
            break;
        case 3 : // Display Current Disposition of visitors
            /*Call a function from Vistor.c that prints all the visitors within the list and their respective CageLocation.
            */

            // TODO: Finish Case 3

            EnterCriticalSection(&ConsoleCrit);

            ConsoleWriteLine(_T("\n%cYou selected - Display Current Disp of Visitors\n\n"), SKYBLUE);

            LeaveCriticalSection(&ConsoleCrit);

            break;
        case 4 : // Show Case Animal
            /*Call a function from Visitor.c that increases the HappinessLevel of all visitors that are currently at a specific cage. This functions should accept a string to be compared with each visitor Visitor->CageLocation. 
            */
            //Call NextTurn()

            // TODO: Finish Case 4

            EnterCriticalSection(&ConsoleCrit);

            ConsoleWriteLine(_T("\n%cYou selected - Show Case Animal\n\n"), SKYBLUE);

            EndTurnActions();

            LeaveCriticalSection(&ConsoleCrit);

            break;
        case 5 : // Check Visitors Happiness Level
            EnterCriticalSection(&ConsoleCrit);

            ConsoleWriteLine(_T("\n%cYou selected - Check Visitors Happ Level\n\n"), SKYBLUE);
            EnumVisitors(visitorListHead, TRUE);

            LeaveCriticalSection(&ConsoleCrit);

            break;
        case 6 :
            EndTurnActions();
            break;
        case 0 :
            ConsoleWriteLine(_T("\n%cYou selected - Quit\n\n"), SKYBLUE);
            EndTurnActions();
            Dispose();
        default :
            ConsoleWriteLine(_T("Invalid Selection...\n"));
            break;
    }
    /*CancelWaitableTimer(hTimer);
    SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);*/

    goto GAMELOOP;
}

//DWORD WINAPI mTimer(LPVOID lpParam) {
//    lpParam = "10";
//    liDueTime.QuadPart = -100000000LL;
//    // Create an unnamed waitable timer.
//    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
//    if (NULL == hTimer) {
//        ConsoleWriteLine(_T("CreateWaitableTimer failed (%d)\n"), GetLastError());
//        return 1;
//    }
//    // Set a timer to wait for 60 seconds.
//    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
//        ConsoleWriteLine(_T("SetWaitableTimer failed (%d)\n"), GetLastError());
//        return 2;
//    }
//    // Wait for the timer.
//mtimerloop:
//    if (tThread != 0)return 0;
//    if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
//        ConsoleWriteLine(_T("WaitForSingleObject failed (%d)\n"), GetLastError());
//    else {
//        ConsoleWriteLine(_T("\n%c------------------------------------\n"),RED);
//        ConsoleWriteLine(_T("%cYou took too long to select your option\nPlease select an option from the menu.\n"),RED);
//
//    }
//    SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
//    goto mtimerloop;
//
//}
