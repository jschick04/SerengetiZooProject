#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include "ConsoleColors.h"
#include "Visitor.h"
#include "Animals.h"
#include <WriteLine.h>
#include <time.h>

//GLOBALS
CRITICAL_SECTION VisitorListCS;
HANDLE hVisitorEvent;
HANDLE hThreadHandles[999];

//FUNCTIONS

//Function is used to signify Zoo Open
HANDLE InitVisitorsEvent()
{
    InitializeCriticalSection(&VisitorListCrit);
    hVisitorEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    SetEvent(hVisitorEvent);
    return hVisitorEvent;
}

//Adds a visitor to the linked list and kicks off a thread to start user loop.
Visitor* AddVisitor(NodeEntry* VisitorListHead, LPTSTR Name)
{
    WaitForSingleObject(hVisitorEvent, INFINITE);
    EnterCriticalSection(&VisitorListCrit);
   //create struct and return values.
    Visitor* NewVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    if (NewVisitor == 0)
    {
        LeaveCriticalSection(&VisitorListCrit);
        //Allocation failed and we need to warn
        Visitor* failed = -1;
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

    LeaveCriticalSection(&VisitorListCrit);
    SetEvent(hVisitorEvent);

    //create the thread to start the visitorloop
    hThreadHandles[VisitorTID]= CreateThread(
        NULL,
        0,
        VisitorLoop,
        Params,
        0,
        dwThreadId[VisitorTID]
    );
    VisitorTID++;
    return NewVisitor;
}

//Removes a visitor from the list of visitors.
Visitor* RemoveVisitor(NodeEntry* VisitorListHead, LPTSTR Name)
{
    WaitForSingleObject(hVisitorEvent, INFINITE);
    EnterCriticalSection(&VisitorListCrit);

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

    LeaveCriticalSection(&VisitorListCrit);
    SetEvent(hVisitorEvent);

    return 0;
}


//This loop is to iterate through each cage, and each animal end to end.
DWORD WINAPI VisitorLoop(VisitorLoopParams* Params)
{
    int SleepTimeRand = 0;
    for (int i = 0; i != _countof(cages); ++i)
    {
        //sleep between 1 and 3 minutes to simulate walk/watch times. 
        SleepTimeRand = (rand() % (180000 - 60000 + 1)) + 60000;
        Sleep(SleepTimeRand);


        //handle error if the cage name is NULL. Something is very wrong, there are no animals.

        if (cages[i]->Name == NULL)
        {
            //WriteConsoleOutput(_T("There are no animals in the zoo left!"),RED);
            ConsoleWriteLine(_T("There are no animals left in the Zoo!\n"));
        }
        else
        {
            Params->Visitor->CageLocation = cages[i]->Name;

            //zookeeper should be alerted after user enterse cage.
            ConsoleWriteLine(_T("%s entered cage: %s\n"), Params->Visitor->UniqueName, Params->Visitor->CageLocation);

            //Get the interactivity level and increase or decrease happiness of visitor. 
            DWORD AverageInterActivityLevel = 0;
            AverageInterActivityLevel = GetCageTotalInteractiveLevel(cages[i]);

            //increase or decrease happiness point based on interctivity level
            if (AverageInterActivityLevel <= 4)
            {
                //reduce one happiness point
                Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel - 1;

            }
            else if (AverageInterActivityLevel == 5)
            {
                //do not add or remove happiness point
                Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel;
            }
            else
            {
                //Add one happiness point
                Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel + 1;
            }

            //after the increase or decrease update status of visitor.
            if (Params->Visitor->HappinessLevel < 5)
            {
                Params->Visitor->HappinessLevel = RefundDemanded;
                
            }
            if (Params->Visitor->HappinessLevel > 5 && Params->Visitor->HappinessLevel <= 7);
            {
                Params->Visitor->HappinessLevel = Disappointed;
                
            }
            if (Params->Visitor->HappinessLevel >= 8)
             {
                Params->Visitor->HappinessLevel = Happy;
             }
        }
    }

    //visitor should have visited all locations at this point so we need to calculate if leaving happy leaving angry or demanding a refund.
    if (Params->Visitor->HappinessLevel < 5)
    {
        Params->Visitor->HappinessLevel = RefundDemanded;
        ConsoleWriteLine(_T("%s visited all cages and left with status: %d\n"), Params->Visitor->UniqueName, Params->Visitor->Status);
    }
    else if (Params->Visitor->HappinessLevel > 5 && Params->Visitor->HappinessLevel <= 7)
    {
        Params->Visitor->HappinessLevel = LeavingAngry;
        ConsoleWriteLine(_T("%s visited all cages and left with status: %d\n"), Params->Visitor->UniqueName, Params->Visitor->Status);
    }
    else
    {
        Params->Visitor->HappinessLevel = LeavingHappy;
        ConsoleWriteLine(_T("%s visited all cages and left with status: %d\n"), Params->Visitor->UniqueName, Params->Visitor->Status);
    }

    Sleep(1000);
    RemoveVisitor(visitorListHead, Params->Visitor->UniqueName);

    //thread should exit at this point.
    return 0;
}

//Function to enumerate all visitors, simply prints them forward.
DWORD WINAPI EnumVisitors(NodeEntry* VisitorListHead, BOOL PrintToConsole)
{
    NodeEntry* EnumNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    EnumNode = VisitorListHead->Flink;
    Visitor* eVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    if (PrintToConsole == TRUE) { ConsoleWriteLine(_T("[  Visitor  ] [  Cage  ] [ Happiness Level ] [  Status  ]\n")); }

    while (EnumNode->Flink != VisitorListHead->Flink)
    {
        eVisitor = CONTAINING_RECORD(EnumNode, Visitor, Links);
        if (PrintToConsole == TRUE) { ConsoleWriteLine(_T("[ %s ] [ %s ] [ %d ] [ %d ]\n"),eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, eVisitor->Status); }
        EnumNode = EnumNode->Flink;
    }

    return 0;
}

//Thread to simulate a random amount of visitors being added periodically.
DWORD WINAPI AddVisitorsThread()
{
    LPTSTR VisitorName[] = {
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
    };

    //int numVisitorsRand = 0;
    //numVisitorsRand = (rand() % (2));

    int SleepRand = 0;
    SleepRand = (rand() % (300000 - 80000 + 1)) + 80000;

    for (int i = 0; i != _countof(VisitorName); ++i)
    {
        Sleep(SleepRand);
        AddVisitor(visitorListHead, VisitorName[i]);
    }

    return 0;
}