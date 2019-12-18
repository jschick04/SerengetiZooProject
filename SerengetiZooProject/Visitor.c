#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include "ConsoleColors.h"
#include "Visitor.h"
#include "Animals.h"
#include <WriteLine.h>

//GLOBALS
CRITICAL_SECTION VisitorListCS;
HANDLE hVisitorEvent;
HANDLE hThreadHandles[999];

//FUNCTIONS

//Function is used to signify Zoo Open
HANDLE InitVisitorsEvent()
{
    hVisitorEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    SetEvent(hVisitorEvent);
    return hVisitorEvent;
}

//Adds a visitor to the linked list and kicks off a thread to start user loop.
Visitor* AddVisitor(NodeEntry* VisitorListHead, LPTSTR Name)
{
   //create struct and return values.
    Visitor* NewVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    if (NewVisitor == 0)
    {
        //Allocation failed and we need to warn
        DWORD failed = -1;
        return failed;
    }

    //update pointer to the first node
    NodeEntry* next;
    next = VisitorListHead->Blink;

    //point the flink to the list head and the blink to the current first node.
    NewVisitor->Links.Blink = next;
    NewVisitor->Links.Flink = VisitorListHead;

    //point current node first node's flink     
    next->Flink = &(NewVisitor->Links);

    //point list head to new node.      
    VisitorListHead->Blink = &(NewVisitor->Links);

    //Add data
    LPTSTR Entry = _T("Entry");

    NewVisitor->UniqueName = Name;
    NewVisitor->CageLocation = Entry;
    NewVisitor->HappinessLevel = 8;
    NewVisitor->Status = Happy;

    VisitorLoopParams* Params = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(VisitorLoopParams));;
    Params->Visitor = NewVisitor;
    Params->listHead = animalListHead;

    //create the thread to start the visitorloop
    dwThreadId[VisitorTID] = CreateThread(
        NULL,
        0,
        VisitorLoop,
        Params,
        0,
        &hThreadHandles[VisitorTID]
    );
    VisitorTID++;
    return NewVisitor;
}

//Removes a visitor from the list of visitors.
Visitor* RemoveVisitor(NodeEntry* VisitorListHead, LPTSTR Name)
{
    NodeEntry* RemovedNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    NodeEntry* TempNodePrev = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    NodeEntry* TempNodeNext = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    RemovedNode = VisitorListHead->Flink;
    Visitor* RemovedVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));
    Visitor* PreviousVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));
    Visitor* NextVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    //loop through all to find a matching uniquename.
    while (RemovedVisitor->UniqueName != Name)
    {
        RemovedVisitor = CONTAINING_RECORD(RemovedNode, Visitor, Links);
        RemovedNode = RemovedNode->Flink;
    }
    //when a unique name is found we need to update the flink of the previous node and the blink of the next node.
    TempNodePrev = RemovedVisitor->Links.Blink;
    PreviousVisitor = CONTAINING_RECORD(TempNodePrev, Visitor, Links);
    TempNodeNext = RemovedVisitor->Links.Flink;
    NextVisitor = CONTAINING_RECORD(TempNodeNext, Visitor, Links);
    PreviousVisitor->Links.Flink = TempNodeNext;
    NextVisitor->Links.Blink = TempNodePrev;

    return 0;
}


//This loop is to iterate through each cage, and each animal end to end.
DWORD WINAPI VisitorLoop(VisitorLoopParams* Params)
{
    AnimalList* CurrentCage = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AnimalList));
    NodeEntry* CurrentNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    CurrentNode = Params->listHead;

    Visitor* Visitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));
    Visitor = Params->Visitor;

    //need to get the current zoo animal struct here implementation not currently present.
    //ZooAnimal* CurrentAnimal = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ZooAnimal));

    srand(1000);

    //loop to go through the entire list of cages
    while (CurrentNode->Flink != Params->listHead)
    {
        if (Visitor->CageLocation != "Entry")
        {
            //Move from Entry to first cage after a sleep timer. This should be a random number between 1 and 3 minutes or so
            Sleep(rand() % 180000);
            CurrentCage = CONTAINING_RECORD(CurrentCage, AnimalList, LinkedList);

            //zookeeper should be alerted after user enterse cage.
            ConsoleWriteLine(_T("%s entered cage: %s"), Visitor->UniqueName, Visitor->CageLocation);

            //Get the interactivity level and increase or decrease happiness of visitor. 
            DWORD AverageInterActivityLevel = 0;
            AverageInterActivityLevel = GetCageTotalInteractiveLevel(Visitor->CageLocation);

            if (AverageInterActivityLevel <= 4)
            {
                //reduce one happiness point
                Visitor->HappinessLevel = Visitor->HappinessLevel - 1;

            }
            else if (AverageInterActivityLevel == 5)
            {
                //do not add or remove happiness point
                Visitor->HappinessLevel = Visitor->HappinessLevel;
            }
            else
            {
                //Add one happiness point
                Visitor->HappinessLevel = Visitor->HappinessLevel + 1;
            }

            //after the increase or decrease update status of visitor.
            if (Visitor->HappinessLevel < 5)
            {
                Visitor->HappinessLevel = RefundDemanded;
                ConsoleWriteLine(_T("%s visited cage %s and left with status: %s"), Visitor->UniqueName, Visitor->CageLocation, Visitor->Status);
            }
            else if (Visitor->HappinessLevel > 5 && Visitor->HappinessLevel <= 7);
            {
                Visitor->HappinessLevel = Disappointed;
                ConsoleWriteLine(_T("%s visited cage %s and left with status: %s"), Visitor->UniqueName, Visitor->CageLocation, Visitor->Status);
            }
            //NOT SURE WHY THIS ELSE ISN'T CATCHING THE IF ABOVE IT.
            //else
            //{
            //    Visitor->HappinessLevel = Happy;
            //    ConsoleWriteLine(_T("%s visited cage %s and left with status: %s"), Visitor->UniqueName, Visitor->CageLocation, Visitor->Status);
            //}

            
            //Move the visitor forward one CAGE.
            CurrentNode = CurrentCage->LinkedList.Flink;
        }
        else 
        {
            Sleep(rand() % 180000);
            CurrentNode = CurrentCage->LinkedList.Flink;
        }

    }

    //visitor should have visited all locations at this point so we need to calculate if leaving happy leaving angry or demanding a refund.
    if (Visitor->HappinessLevel < 5)
    {
        Visitor->HappinessLevel = RefundDemanded;
        ConsoleWriteLine(_T("%s made visited all cages and left with status: %s"), Visitor->UniqueName, Visitor->Status);
    }
    else if (Visitor->HappinessLevel > 5 && Visitor->HappinessLevel <= 7)
    {
        Visitor->HappinessLevel = LeavingAngry;
        ConsoleWriteLine(_T("%s made visited all cages and left with status: %s"), Visitor->UniqueName, Visitor->Status);
    }
    else
    {
        Visitor->HappinessLevel = LeavingHappy;
        ConsoleWriteLine(_T("%s made visited all cages and left with status: %s"), Visitor->UniqueName, Visitor->Status);
    }

    //need to remove visitor from linked list here.
    RemoveVisitor(visitorListHead, Visitor->UniqueName);

    //thread should exit at this point.
    return 0;
}

//Function to enumerate all visitors, simply prints them forward.
DWORD WINAPI EnumVisitors(NodeEntry* VisitorListHead, BOOL PrintToConsole)
{
    NodeEntry* EnumNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    EnumNode = VisitorListHead->Flink;
    Visitor* eVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    if (PrintToConsole == TRUE) { ConsoleWriteLine(_T("[  Visitor  ] [  Cage Location ] [ Happiness Level ] [  Status  ]\n")); }

    while (EnumNode->Flink != VisitorListHead->Flink)
    {
        eVisitor = CONTAINING_RECORD(EnumNode, Visitor, Links);
        if (PrintToConsole == TRUE) { ConsoleWriteLine(_T("[  %s  ] [  %s  ] [    %d    ] [  %d  ]\n"),eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, eVisitor->Status); }
        EnumNode = EnumNode->Flink;
    }

    return 0;
}