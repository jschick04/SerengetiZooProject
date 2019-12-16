#include <Windows.h>
#include <tchar.h>
#include <WriteLine.h>
#include <strsafe.h>
#include <stdlib.h>
#include <ConsoleColors.h>
#include "Visitor.h"

#define MAXS 20;
#define IS_LIST_EMPTY(listHead) ((listHead)->blink == (listHead));


enum VisitorStatus { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

typedef struct Visitor {
    LPTSTR UniqueName;
    LPTSTR CageLocation;
    DWORD HappinessLevel;
    enum VisitorStatus Status;
    struct NodeEntry Links;
} Visitor;

int mTurns = 15;
int Score = 0;

void printScore();
void printvHappinessLevel();
void NextTurn();

NodeEntry* animalListHead = 0;
NodeEntry* visitorListHead = 0;

CRITICAL_SECTION cScore;
CRITICAL_SECTION AnimalListCrit;

BOOL InitializeListHead() {
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
    if (InitializeListHead() == FALSE) {
        ConsoleWriteLine(_T("%cFailed to create list heads"), RED);
        return -1;
    }

    if (InitializeCriticalSectionAndSpinCount(&AnimalListCrit, 4000) == FALSE) {
        ConsoleWriteLine(_T("%cFailed to create CRITICAL_SECTION"), RED);
        return -1;
    }

    //Initialize Threads to call Visitor.c and Animals.c
    //Wait on these threads to have finished and then present the user with the game menu.

    ConsoleWriteLine(_T("Please select your action"), WHITE);
    int Menu;
    ConsoleWriteLine(_T("1 - Create a visitor\n2 - Feed Animals\n3 - Check Animal Interactivity Levels\n4 - Display Current Disposition of visitors\n5 - Show Case Animal\n6 - Check Visitors Happiness Level\n7 - Turn"), WHITE);
    scanf_s("%d", &Menu, 1);
GAMELOOP:
    switch (Menu) {
    case 1: // Create a visitor
        char vName[MAXS] = fgets(vName, MAXS, stdin);
        //call visitor creation sending value at vName
        //Calls NextTurn()
        break;
    case 2: // Feed Animal
        //enum animals and print health levels for each animal
        NodeEntry* temp = animalListHead->Flink;
        ZooAnimal* base;
        if (IsListEmpty(animalListHead))
        {
            ConsoleWriteLine(_T("The Animal list is empty\n"), RED);
            return;
        }
        while (temp != animalListHead)
        {
            base = CONTAINING_RECORD(temp,      //pointer to the field (the field name is param 3 below)
                ZooAnimal,    //name of structure we want the base of
                ActiveDataLinks.Flink); //field name of object we want the base of
            ConsoleWriteLine(_T("Animal:  %s - Health Level: %d \n", base->UniqueName, base->HealthLevel));
            temp = temp->Flink;
        }
        ConsoleWriteLine(_T("Type the name of the animal you want to feed\n"), WHITE);
        char aName[MAXS] = fgets(vName, MAXS, stdin);
        while (temp != animalListHead)
        {
            base = CONTAINING_RECORD(temp,      //pointer to the field (the field name is param 3 below)
                ZooAnimal,    //name of structure we want the base of
                ActiveDataLinks.Flink); //field name of object we want the base of
            if (aName == base->UniqueName) {
                //call function to increase Health level.
            }
            temp = temp->Flink;
        }
        //Calls NextTurn()
        break;
    case 3: // Check Animal Interactivity Levels
        NodeEntry* temp = animalListHead->Flink;
        ZooAnimal* base;
        if (IsListEmpty(animalListHead))
        {
            ConsoleWriteLine(_T("The Animal list is empty\n"), RED);
            return;
        }
        while (temp != animalListHead)
        {
            base = CONTAINING_RECORD(temp,      //pointer to the field (the field name is param 3 below)
                ZooAnimal,    //name of structure we want the base of
                ActiveDataLinks.Flink); //field name of object we want the base of
            ConsoleWriteLine(_T("Animal:  %s - Interactivity Level: %d \n", base->UniqueName, base->InteractiveLevel));
            temp = temp->Flink;
        }
        break;
    case 4: // Display Current Disposition of visitors
        NodeEntry* temp = visitorListHead->Flink;
        ZooAnimal* base;
        if (IsListEmpty(visitorListHead))
        {
            ConsoleWriteLine(_T("The Visitor list is empty\n"), RED);
            return;
        }
        while (temp != visitorListHead)
        {
            base = CONTAINING_RECORD(temp,      //pointer to the field (the field name is param 3 below)
                ZooAnimal,    //name of structure we want the base of
                ActiveDataLinks.Flink); //field name of object we want the base of
            ConsoleWriteLine(_T("Vistor:  %s - Currently at cage: %d \n", base->UniqueName, base->CageLocation));
            temp = temp->Flink;
        }
        break;
    case 5: // Show Case Animal
        NodeEntry* temp = animalListHead->Flink;
        ZooAnimal* base;
        if (IsListEmpty(animalListHead))
        {
            ConsoleWriteLine(_T("The Animal list is empty\n"), RED);
            return;
        }
        ConsoleWriteLine(_T("Type the name of the animal you want to showcase\n"), WHITE);
        char aName[MAXS] = fgets(vName, MAXS, stdin);
        while (temp != animalListHead)
        {
            base = CONTAINING_RECORD(temp,      //pointer to the field (the field name is param 3 below)
                ZooAnimal,    //name of structure we want the base of
                ActiveDataLinks.Flink); //field name of object we want the base of
            if (aName == base->UniqueName) {
                //call function to increase visitor happiness level.
            }
            temp = temp->Flink;
        }
        //Call NextTurn()
        break;
    case 6: // Check Visitors Happiness Level
        NodeEntry* temp = visitorListHead->Flink;
        Visitor* base;
        if (IsListEmpty(visitorListHead))
        {
            ConsoleWriteLine(_T("The Animal list is empty\n"), RED);
            return;
        }
        ConsoleWriteLine(_T("Type the name of the animal you want to showcase\n"), WHITE);
        char aName[MAXS] = fgets(vName, MAXS, stdin);
        while (temp != visitorListHead)
        {
            base = CONTAINING_RECORD(temp,      //pointer to the field (the field name is param 3 below)
                Visitor,    //name of structure we want the base of
                ActiveDataLinks.Flink); //field name of object we want the base of
            if (aName == base->UniqueName) {
                //call function to increase visitor happiness level.
            }
            temp = temp->Flink;
        }
        break;
    case 7: //Next Turn
        //Calls NextTurn() function which signal Visitors and Animals that they can move one step forward.
        //Print current score and Happiness Level.
        break;
    default:
        break;
        goto GAMELOOP;
    }

    HeapFree(GetProcessHeap(), 0, &animalListHead);
    HeapFree(GetProcessHeap(), 0, &visitorListHead);
}

void printScore() {
    ConsoleWriteLine(_T("Your current score is %d\n"), Score, BLUE);

}

void printvHappinessLevel() {
    ConsoleWriteLine(_T("Your current score is %d\n"), Score, BLUE);
}

void NextTurn() {

}
