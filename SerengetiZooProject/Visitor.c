#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include "ConsoleColors.h"
#include "Visitor.h"
#include "Animals.h"
#include <WriteLine.h>
#include <time.h>
#include <strsafe.h>

//GLOBALS
CRITICAL_SECTION VisitorListCS;
HANDLE hVisitorEvent;
BOOL bExitZoo;

//FUNCTIONS

//Function is used to signify Zoo Open
HANDLE InitVisitorsEvent()
{
    InitializeCriticalSection(&VisitorListCrit);
    hVisitorEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hVisitorEvent != NULL)
    {
       SetEvent(hVisitorEvent);
    }
    VisitorEnterEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (VisitorEnterEvent != NULL)
    {
        SetEvent(VisitorEnterEvent);
    }
    
    return hVisitorEvent;
}

//Adds a visitor to the linked list and kicks off a thread to start user loop.
Visitor* AddVisitor(NodeEntry* VisitorListHead, LPTSTR Name)
{
    WaitForSingleObject(hVisitorEvent, INFINITE);
    EnterCriticalSection(&VisitorListCrit);
   //create struct and return values.
    Visitor* NewVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

if (NewVisitor == NULL)
{
    LeaveCriticalSection(&VisitorListCrit);
    SetEvent(hVisitorEvent);
    //Allocation failed and we need to warn
    Visitor* failed = NULL;
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
    if (Params != NULL)
    {
    Params->Visitor = NewVisitor;
    Params->listHead = animalListHead;
    }

LeaveCriticalSection(&VisitorListCrit);
SetEvent(hVisitorEvent);

//create the thread to start the visitorloop
hThreadHandles[VisitorTID] = CreateThread(
    NULL,
    0,
    VisitorLoop,
    Params,
    0,
    dwThreadId[VisitorTID]
);
if (hThreadHandles[VisitorTID] == NULL) {
    ConsoleWriteLine(_T("%cFailed to spawn thread\n"), RED, GetLastError());
    return NULL;
}
VisitorTID++;
return NewVisitor;
}

//Removes a visitor from the list of visitors.
Visitor* RemoveVisitor(NodeEntry* VisitorListHead, LPTSTR Name)
{
    WaitForSingleObject(hVisitorEvent, INFINITE);
    EnterCriticalSection(&VisitorListCrit);

    NodeEntry* RemovedNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    if (RemovedNode == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        LeaveCriticalSection(&VisitorListCrit);
        SetEvent(hVisitorEvent);
        return NULL;
    }
    NodeEntry* TempNodePrev = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    if (TempNodePrev == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        LeaveCriticalSection(&VisitorListCrit);
        SetEvent(hVisitorEvent);
        return NULL;
    }
    NodeEntry* TempNodeNext = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    if (TempNodeNext == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        LeaveCriticalSection(&VisitorListCrit);
        SetEvent(hVisitorEvent);
        return NULL;
    }
    RemovedNode = VisitorListHead->Flink;
    Visitor* RemovedVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));
    if (RemovedVisitor == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        LeaveCriticalSection(&VisitorListCrit);
        SetEvent(hVisitorEvent);
        return NULL;
    }
    Visitor* PreviousVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));
    if (PreviousVisitor == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        LeaveCriticalSection(&VisitorListCrit);
        SetEvent(hVisitorEvent);
        return NULL;
    }
    Visitor* NextVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));
    if (NextVisitor == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        LeaveCriticalSection(&VisitorListCrit);
        SetEvent(hVisitorEvent);
        return NULL;
    }

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

    //cleanup
    HeapFree(GetProcessHeap(), 0, RemovedVisitor);

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
        int sleeparray[5];
        for (int s = 0; s != 5; ++s)
        {
            sleeparray[s] = SleepTimeRand = (rand() % (1800 - 600)) + 600;
        }       
        int selector = (rand() % 5);

        //sleep while waking to check if exit zoo is true.
        for (int t = 0; t != 10; ++t)
        {
            if (bExitZoo == TRUE)
            {
                //LeaveCriticalSection(&VisitorListCrit);
                break;
            }
            else
            {
                Sleep(sleeparray[selector]);
            }

        }
        

        //handle error if the cage name is NULL. Something is very wrong, there are no animals.
        if (IsCageEmpty(cages[i]->Name))
        {
           //WriteConsoleOutput(_T("There are no animals in the zoo left!"),RED);
            //ConsoleWriteLine(_T("There are no animals left in the Zoo!\n"));
        }
        else
        {
            WaitForSingleObject(hVisitorEvent, INFINITE);
            EnterCriticalSection(&VisitorListCrit);

            Params->Visitor->CageLocation = cages[i]->Name;
            ConsoleWriteLine(_T("%s entered cage: %s\n"), Params->Visitor->UniqueName, Params->Visitor->CageLocation);

            if (IsCageEmpty(Params->Visitor->CageLocation) == TRUE)
            {
                Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel - 2;
                ConsoleWriteLine(_T("%s lost 2 happiness points after visiting %s due to being empty!\n"), Params->Visitor->UniqueName, Params->Visitor->CageLocation);
            }
            else
            {
                //Get the interactivity level and increase or decrease happiness of visitors
                DWORD AverageInterActivityLevel = 0;
                AverageInterActivityLevel = GetCageTotalInteractiveLevel(cages[i]->Name);

                //increase or decrease happiness point based on interctivity level
                if (AverageInterActivityLevel <= 4)
                {
                    if (Params->Visitor->HappinessLevel != 0)
                    {
                    Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel - 1;
                    ConsoleWriteLine(_T("%s %clost%r a happiness point after visiting %s!\n"), Params->Visitor->UniqueName, RED, Params->Visitor->CageLocation);
                    }

                }
                else if (AverageInterActivityLevel == 5)
                {
                    Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel;
                }
                else
                {
                    if (Params->Visitor->HappinessLevel != 10)
                    { 
                    Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel + 1;
                    ConsoleWriteLine(_T("%s %cgained%r a happiness point after visiting %s!\n"), Params->Visitor->UniqueName, GREEN,  Params->Visitor->CageLocation);
                    }
                }
            }

            //after the increase or decrease update status of visitor.
            if (Params->Visitor->HappinessLevel < 5)
            {
                Params->Visitor->Status = RefundDemanded;
                LeaveCriticalSection(&VisitorListCrit);
                SetEvent(hVisitorEvent);
                break;
                
            }
            if (Params->Visitor->HappinessLevel > 5 && Params->Visitor->HappinessLevel <= 7);
            {
                Params->Visitor->Status = Disappointed;
                
            }
             if (Params->Visitor->HappinessLevel >= 8)
             {
                Params->Visitor->Status = Happy;
             }

             if (bExitZoo == TRUE)
            {
                 LeaveCriticalSection(&VisitorListCrit);
                 SetEvent(hVisitorEvent);
                 break;
            }
             LeaveCriticalSection(&VisitorListCrit);
             SetEvent(hVisitorEvent);
        }
    }

    Sleep(SleepTimeRand);
    //visitor should have visited all locations at this point so we need to calculate if leaving happy leaving angry or demanding a refund.
    LPTSTR ExitStatus;
    WaitForSingleObject(hVisitorEvent, INFINITE);
    EnterCriticalSection(&VisitorListCrit);

    if (Params->Visitor->HappinessLevel < 5)
    {
        Params->Visitor->HappinessLevel = RefundDemanded;
        ExitStatus = _T("Demanded a Refund");
        ConsoleWriteLine(_T("%s %c%s\n"), Params->Visitor->UniqueName, RED, ExitStatus);
    }
    else if (Params->Visitor->HappinessLevel > 5 && Params->Visitor->HappinessLevel <= 7)
    {
        Params->Visitor->HappinessLevel = LeavingAngry;
        ExitStatus = _T("Left Angry");
        ConsoleWriteLine(_T("%s %c%s\n"), Params->Visitor->UniqueName, RED, ExitStatus);
        g_Score = g_Score - 10;
    }
    else
    {
        Params->Visitor->HappinessLevel = LeavingHappy;
        ExitStatus = _T("Left Happy");
        ConsoleWriteLine(_T("%s %c%s\n"), Params->Visitor->UniqueName, GREEN, ExitStatus);
        g_Score = g_Score + 10;
        
    }

    LeaveCriticalSection(&VisitorListCrit);
    SetEvent(hVisitorEvent);
    Sleep(1000);
    RemoveVisitor(visitorListHead, Params->Visitor->UniqueName);

    return 0;
}

     //Function to enumerate all visitors, simply prints them forward.
    DWORD WINAPI GetVisitorCount(NodeEntry * VisitorListHead)
    {
        DWORD Count = 0;
        WaitForSingleObject(hVisitorEvent, INFINITE);
        EnterCriticalSection(&VisitorListCrit);

        NodeEntry* EnumNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
        if (EnumNode == NULL) {
            ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        }
        EnumNode = VisitorListHead->Flink;

        while (EnumNode->Flink != VisitorListHead->Flink)
        {
            Count++;
            EnumNode = EnumNode->Flink;
        }

        LeaveCriticalSection(&VisitorListCrit);
        SetEvent(hVisitorEvent);

        return Count;
    }

    DWORD WINAPI ExitZoo()
    {
        bExitZoo = TRUE;
        ResetEvent(VisitorEnterEvent);
        
        return 1;
    }

    DWORD WINAPI EnterZoo()
    {
        bExitZoo = FALSE;
        SetEvent(VisitorEnterEvent);
        return 1;
    }

    //Function to enumerate all visitors, simply prints them forward.
DWORD WINAPI EnumVisitors(NodeEntry* VisitorListHead, BOOL PrintToConsole)
{

    WaitForSingleObject(hVisitorEvent, INFINITE);
    EnterCriticalSection(&VisitorListCrit);

    NodeEntry* EnumNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    if (EnumNode == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        //return NULL;
    }
    EnumNode = VisitorListHead->Flink;
    Visitor* eVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    if (PrintToConsole == TRUE) { ConsoleWriteLine(_T("[ Visitor ] [ Cage ] [ Happiness ] [ Status ]\n")); }

    while (EnumNode->Flink != VisitorListHead->Flink)
    {
        LPTSTR status = 0;
        eVisitor = CONTAINING_RECORD(EnumNode, Visitor, Links);

        if (eVisitor->Status == 0)
        {
            status = _T("Happy");
        }

        else if (eVisitor->Status == 1)
        {
            status = _T("Disappointed");
        }

        else if (eVisitor->Status == 2)
        {
            status = _T("RefundDemanded");
        }

        //perform the console print.
        if (PrintToConsole == TRUE && eVisitor->HappinessLevel <= 5) 
        { 
            ConsoleWriteLine(_T("[ %c%s   -  %s  -  %d  -  %s  ]\n"),RED, eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, status); 
        }
        else if (PrintToConsole == TRUE && eVisitor->HappinessLevel >= 8)
        {
            ConsoleWriteLine(_T("[ %c%s   -  %s  -  %d  -  %s  ]\n"),GREEN, eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, status);
        }
        else
        {
            ConsoleWriteLine(_T("[ %c%s   -  %s  -  %d  -  %s  ]\n"),YELLOW, eVisitor->UniqueName, eVisitor->CageLocation, eVisitor->HappinessLevel, status);
        }

        EnumNode = EnumNode->Flink;
    }

    LeaveCriticalSection(&VisitorListCrit);
    SetEvent(hVisitorEvent);


    return 0;
}

//Thread to simulate a random amount of visitors being added periodically.
DWORD WINAPI AddVisitorsThread(BOOL* go)
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
        
        int SleepRand = 0;
        int i = 0;
        int num = 0;
        int numVisitorsRand = 0;

        //Sleep at the begginning to delay after initial seed.
        SleepRand = (rand() % (300000 - 80000 + 1)) + 80000;
        if (i != 0)
        { 
        Sleep(SleepRand);
        }
        *go = TRUE;

        //Determine number of visitors to add
        while(1)
        {
            if (bExitZoo != TRUE)
            { 
            WaitForSingleObject(VisitorEnterEvent, INFINITE);
            numVisitorsRand = (rand() % 3);
            for (num = 0; num != numVisitorsRand; ++num)
            {
                AddVisitor(visitorListHead, VisitorName[i]);
                ++i;
            }
            if (i == _countof(VisitorName))
            {
                // go back to the beginning of the name list.
                i = 0;
            }
            SleepRand = (rand() % (30000 - 8000 + 1)) + 8000;
            Sleep(SleepRand);
            }
            else
            {
                Sleep(1000);
            }
        }

    return 0;
}

//Function to enumerate all visitors, simply prints them forward.
DWORD WINAPI ShowCaseAnimal(NodeEntry* VisitorListHead, int cagenum)
{
    const LPTSTR cageName = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(TCHAR) * 10);
    const LPTSTR prepend = _T("Cage");
    if (cageName != 0)
    {
        StringCchPrintf(cageName, 10, _T("%s%d"), prepend, cagenum);
    }

    WaitForSingleObject(hVisitorEvent, INFINITE);
    EnterCriticalSection(&VisitorListCrit);

    NodeEntry* EnumNode = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry));
    if (EnumNode == NULL) {
        ConsoleWriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
        //return NULL;
    }
    EnumNode = VisitorListHead->Flink;
    Visitor* eVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    while (EnumNode->Flink != VisitorListHead->Flink)
    {
        eVisitor = CONTAINING_RECORD(EnumNode, Visitor, Links);
        if (eVisitor->CageLocation == cageName)
        {
            eVisitor->HappinessLevel = eVisitor->HappinessLevel + 1;
        }

        EnumNode = EnumNode->Flink;
    }

    LeaveCriticalSection(&VisitorListCrit);
    SetEvent(hVisitorEvent);


    return 0;
}