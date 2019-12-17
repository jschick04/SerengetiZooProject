#include <windows.h>
#include <tchar.h>
#include "main.c"
#include "ConsoleColors.h"
#include "Visitor.h"

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
Visitor* AddVisitor(VisitorNodeEntry* VisitorListHead, const char* Name)
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
    char Entry[] = "Entry";
   
    NewVisitor->UniqueName = Name;
    NewVisitor->CageLocation = Entry;
    NewVisitor->HappinessLevel = 8;
    NewVisitor->Status = Happy;

    //LeaveCriticalSection(&VisitorListCS);
    SetEvent(hVisitorEvent);

    //Start the Thread for the Visitor Loop HERE.

    return NewVisitor;
}

//This loop is to iterate through each cage, and each animal end to end.
DWORD WINAPI VisitorLoop(Visitor* Visitor, AnimalList* Animals)
{
    AnimalList* CurrentCage = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(AnimalList));
    CurrentCage = Animals;

    //need to get the current zoo animal struct here implementation not currently present.
    ZooAnimal* CurrentAnimal = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ZooAnimal));

    srand(NULL);

    //loop to go through the entire list of cages
    while (CurrentCage->LinkedList.Flink != NULL)
    {
        //Move from Entry to first cage after a sleep timer. This should be a random number between 1 and 3 minutes or so
        sleep(rand() % 180000);

        //loop thorugh all the animals in each cage.
        while (CurrentAnimal->AnimalType == Visitor->CageLocation)
        {
            //Get the next animal in the cage.

        }
        
    }
    return Visitor;
}