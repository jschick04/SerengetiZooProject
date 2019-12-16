#include <Windows.h>
#include <string.h>
#include <tchar.h>
#include <stdlib.h>
#include "../SerengetiZooProject/main.c"

//Adds a visitor to the linked list.
int AddVisitor(NodeEntry* VisitorListHead) {

    //Allocate heaps for the new visitor and node entries.
    Visitor* NewVisitor = NULL;
    NewVisitor = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor));

    //Determine if list is empty
    if (visitorListHead == NULL)
    {
        //set the list head to the new visitor
        visitorListHead = &(NewVisitor->Links);

        // set the FLINK to NULL and the Blink to the List Head.
        NewVisitor->Links.Flink = NULL;
        NewVisitor->Links.Blink = VisitorListHead;
    }

    //otherwise find the end of the list add the entry there.
    else
    {
        NewVisitor->Links = &VisitorListHead;
        while (NewVisitor->Links.Flink != NULL)
        {
            NewVisitor->Links.Flink = NewVisitor->Links.Flink
        }


    }


}

int RemoveVisitor(void* VisitorListHead, LPTSTR VisitorName) {



}

int ChangeLocation(void* VisitorListHead, LPTSTR VisitorName) {



}

int ChangeHappiness(void* VisitorListHead, LPTSTR VisitorName, int Amount) {



}

int ChangeStatus(void* VisitorListHead, LPTSTR VisitorName, LPTSTR NewStatus) {



}