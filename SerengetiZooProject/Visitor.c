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
DWORD dwThreadIds[999];


//FUNCTIONS

//Function is used to signify Zoo Open
HANDLE InitVisitorsEvent()
{
    hVisitorEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    return hVisitorEvent;
}

//Adds a visitor to the linked list and kicks off a thread to start user loop.
Visitor* AddVisitor(NodeEntry* VisitorListHead, LPTSTR Name)
{
    WaitForSingleObject(hVisitorEvent, INFINITE);
    //EnterCriticalSection(&VisitorListCS);

    Visitor* NewVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    //Determine if List is Empty or not.
    if (VisitorListHead->Flink == NULL)
    {
        //set the flink to the new entry.
        VisitorListHead->Flink = &NewVisitor->Links;

        //Set the Blink to the list head and the flink to null for the new visitor
        NewVisitor->Links.Blink = VisitorListHead;
        NewVisitor->Links.Flink = NULL;

    }

    //otherwise find the last entry in list and put the visitor at the end.
    else
    {
        NewVisitor->Links = *VisitorListHead;

        while (NewVisitor->Links.Flink != NULL)
        {
            NewVisitor->Links = *NewVisitor->Links.Flink;
        }

        //save the previous info.
        Visitor* PreviousVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));
        PreviousVisitor->Links = NewVisitor->Links;

        //Set the Blink to the previous node and the flink to null for the new visitor
        NewVisitor->Links.Blink = &(PreviousVisitor->Links);
        NewVisitor->Links.Flink = NULL;

        //point the flink of the previous visitor at the new Visitor.
        PreviousVisitor->Links.Flink = &(NewVisitor->Links);

    }

    //links should be updated at this point, data should be set here.

    LPTSTR Entry = _T("Test");
   
    NewVisitor->UniqueName = Name;
    NewVisitor->CageLocation = Entry;
    NewVisitor->HappinessLevel = 8;
    NewVisitor->Status = Happy;

    //LeaveCriticalSection(&VisitorListCS);
    SetEvent(hVisitorEvent);

    //Start the Thread for the Visitor Loop HERE.

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
DWORD WINAPI VisitorLoop(Visitor* Visitor, AnimalList* Animals)
{
    AnimalList* CurrentCage = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AnimalList));
    CurrentCage = Animals;

    //need to get the current zoo animal struct here implementation not currently present.
    //ZooAnimal* CurrentAnimal = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ZooAnimal));

    srand(1000);

    //loop to go through the entire list of cages
    while (CurrentCage->LinkedList.Flink != NULL)
    {
        //Move from Entry to first cage after a sleep timer. This should be a random number between 1 and 3 minutes or so
        Sleep(rand() % 180000);

        //zookeeper should be alerted after user enterse cage.
        ConsoleWriteLine(_T("%s entered cage: %s"), Visitor->UniqueName, Visitor->CageLocation );

        //Get the interactivity level and increase or decrease happiness of visitor. 
        DWORD AverageInterActivityLevel;
        AverageInterActivityLevel = GetInteractiveLevel(Visitor->CageLocation);

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

    if (PrintToConsole == TRUE) { ConsoleWriteLine(_T("[  Visitor  ] [  Cage Location ] [ Happiness Level ] [  Status  ]")); }

    while (EnumNode->Flink != VisitorListHead)
    {
        eVisitor = CONTAINING_RECORD(EnumNode, Visitor, Links);
        if (PrintToConsole == TRUE) { ConsoleWriteLine(_T("[  %s  ] [  %s  ] [    %d    ] [  %s  ]"),eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, eVisitor->Status); }
        EnumNode = EnumNode->Flink;
    }

    return 0;
}