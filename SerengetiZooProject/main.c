#include "SerengetiZooProject.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <WriteLine.h>
#include <ConsoleColors.h>

#define MAXS 20
#define MAXA 5

NodeEntry* animalListHead = 0;
NodeEntry* visitorListHead = 0;

g_Score = 10;
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

int _tmain() {
    if (InitializeListHeads() == FALSE) {
        ConsoleWriteLine(_T("%cFailed to create list heads\n"), RED);
        return -1;
    }

    if (!InitializeCriticalSectionAndSpinCount(&AnimalListCrit, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Animal List CRITICAL_SECTION"), RED);
    }
    if (!InitializeCriticalSectionAndSpinCount(&cScore, 4000)) {
        ConsoleWriteLine(_T("%cFailed to create Score CRITICAL_SECTION"), RED);
    }

    TCHAR buffer[MAX_PATH];
    int menuOption;
    
    //Initialize animals structs.
    char aTypeinit[MAXA][15] =
    {
        "Antelopes",
        "Giraffes",
        "Hyaena",
        "Hippos",
        "Monkeys",
    };
    char aUniqueinit[MAXA][15] =
    {
        "Julien",
        "Melman",
        "Maurice",
        "Gloria",
        "Mason",
    };
    DWORD interactiveLevel[MAXA];
    for (int i = 0; i != MAXA; i++) {
        interactiveLevel[i] = (rand() % (6 - 4 + 1)) + 4;
        NewAnimal(aTypeinit[i], aUniqueinit[i], aTypeinit[i], interactiveLevel);
    }
    DWORD tid = 0;
    HANDLE ht;
    ht = CreateThread(NULL, 0, mTimer, 0, 0, &tid);
GAMELOOP:

    ConsoleWriteLine(_T("Please select your action\n"));
    ConsoleWriteLine(_T("-------------------------\n"));
    ConsoleWriteLine(_T("1 - Feed Animals\n"));
    ConsoleWriteLine(_T("2 - Check Animal Interactivity Levels\n"));
    ConsoleWriteLine(_T("3 - Display Current Disposition of visitors\n"));
    ConsoleWriteLine(_T("4 - Show Case Animal\n"));
    ConsoleWriteLine(_T("5 - Check Visitors Happiness Level\n"));
    ConsoleWriteLine(_T("6 - Turn\n"));
    ConsoleWriteLine(_T("0 - Exit\n"));

    

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
            ConsoleWriteLine(_T("%cYou selected - Feed Animals\n"),BLUE);

            break;
        case 2 : // Check Animal Interactivity Levels
            /*Call a function from Animal.c that prints all the animals within the list and their respective Interactivity Levels.
            */
            ConsoleWriteLine(_T("%cYou selected - Check Animal Int Levels\n"),BLUE);
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
            break;
        case 6 : //Next Turn
            //Calls NextTurn() function which signal Visitors and Animals that they can move one step forward.
            //Print current score and Happiness Level.
            ConsoleWriteLine(_T("%cYou selected - Next Turn\n"),BLUE);
            break;
        case 0 :
            ConsoleWriteLine(_T("%cYou selected - Quit\n"),BLUE);
            goto QUIT;
            // Quit logic
            break;
        default :
            ConsoleWriteLine(_T("Invalid Selection...\n"));
            break;
    }
    CancelWaitableTimer(hTimer);
    SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
    printScore();
    goto GAMELOOP;
    QUIT:
    CancelWaitableTimer(hTimer);
    //if(ht)TerminateThread(ht,0);
    tThread = 1;
    HeapFree(GetProcessHeap(), 0, &animalListHead);
    HeapFree(GetProcessHeap(), 0, &visitorListHead);
}

DWORD WINAPI mTimer(LPVOID lpParam) {
    lpParam = "10";
    liDueTime.QuadPart = -100000000LL;
    // Create an unnamed waitable timer.
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (NULL == hTimer)
    {
        printf("CreateWaitableTimer failed (%d)\n", GetLastError());
        return 1;
    }
    // Set a timer to wait for 60 seconds.
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
    {
        printf("SetWaitableTimer failed (%d)\n", GetLastError());
        return 2;
    }
    // Wait for the timer.
    mtimerloop:
    if (tThread != 0)return 0;
    if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
        printf("WaitForSingleObject failed (%d)\n", GetLastError());
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