#include <windows.h>
#include <tchar.h>
#include "main.c"
#include "ConsoleColors.h"

//Globals.


//FUNCTIONS

Visitor* AddVisitor(NodeEntry* VisitorListHead, const char* Name, HANDLE hVisitorEvent, HANDLE hVisitorThread)
{
    WaitForSingleObject(hVisitorEvent, INFINITE);
    EnterCriticalSection(&VisitorListCS);

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
    char Location[] = "Entry";

    NewVisitor->UniqueName = Name;
    NewVisitor->CageLocation = Location;
    NewVisitor->HappinessLevel = 8;
    NewVisitor->Status = Happy;

    LeaveCriticalSection(&VisitorListCS);
    SetEvent(hVisitorEvent);

    //Start the Thread for the Visitor Loop HERE.

    return NewVisitor;
}



