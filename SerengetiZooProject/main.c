#include "SerengetiZooProject.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <WriteLine.h>
#include <ConsoleColors.h>

#define IS_LIST_EMPTY(listHead) ((listHead)->blink == (listHead));
#define MAXS 20;

void printScore();
void printvHappinessLevel();
void NextTurn();

NodeEntry* animalListHead = 0;
NodeEntry* visitorListHead = 0;

int g_Score = 0;

int mTurns = 15;

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

    InitializeCriticalSectionAndSpinCount(&AnimalListCrit, 4000);
    InitializeCriticalSectionAndSpinCount(&cScore, 4000);

    // TODO: Implement Main Loop
    ConsoleWriteLine(_T("Please select your action\n"));
    int Menu;
    ConsoleWriteLine(_T("1 - Create a visitor\n"));
    ConsoleWriteLine(_T("2 - Feed Animals\n"));
    ConsoleWriteLine(_T("3 - Check Animal Interactivity Levels\n"));
    ConsoleWriteLine(_T("4 - Display Current Disposition of visitors\n"));
    ConsoleWriteLine(_T("5 - Show Case Animal\n"));
    ConsoleWriteLine(_T("6 - Check Visitors Happiness Level\n"));
    ConsoleWriteLine(_T("7 - Turn\n"));
    scanf_s("%d", &Menu, 1);
GAMELOOP:
    switch (Menu) {
        case 1 : // Create a visitor
            /*Call a function from Visitor.c that triggers the creation of a visitor. This functions should accept a string to fill Visitor->UniqueName. The creation of the visitor should also define:
            Visitor->CageLocation to 1
            Visitor->HappinessLevel to {Need some input from you guys here, I thought this could be a random number between 4-6 just to add some Game Inteliggence.
            Visitor->Status to NULL
            */
            break;
        case 2 : // Feed Animal
            /*Call a function from Animal.c that prints all the animals within the list and their respective health level.
            Call a function from Animal.c that triggers the feeding of a particular animal within the list. This functions should accept a string that would be compared to Animal->UniqueName
            In order to be compliant with the code requirements, this function should also check if the feedevent has been set, I will set the event and then call the feed function passing the animal name as a string.
            */
            break;
        case 3 : // Check Animal Interactivity Levels
            /*Call a function from Animal.c that prints all the animals within the list and their respective Interactivity Levels.
            */
            break;
        case 4 : // Display Current Disposition of visitors
            /*Call a function from Vistor.c that prints all the visitors within the list and their respective CageLocation.
            */
            break;
        case 5 : // Show Case Animal
            /*Call a function from Visitor.c that increases the HappinessLevel of all visitors that are currently at a specific cage. This functions should accept a string to be compared with each visitor Visitor->CageLocation. 
            */
            //Call NextTurn()
            break;
        case 6 : // Check Visitors Happiness Level
            /*Call a function from Vistor.c that prints all the visitors within the list and their respective HappinessLevel.
            */
            break;
        case 7 : //Next Turn
            //Calls NextTurn() function which signal Visitors and Animals that they can move one step forward.
            //Print current score and Happiness Level.
            break;
        default :
            break;
            goto GAMELOOP;
    }

    HeapFree(GetProcessHeap(), 0, &animalListHead);
    HeapFree(GetProcessHeap(), 0, &visitorListHead);
}
