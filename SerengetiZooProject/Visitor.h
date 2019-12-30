#ifndef Visitor_H
#define Visitor_H
#include <Windows.h>
#include "SerengetiZooProject.h"
#include "Animals.h"

enum VisitorStatus { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

typedef struct Visitor {
    LPTSTR UniqueName;
    LPTSTR CageLocation;
    DWORD HappinessLevel;
    enum VisitorStatus Status;
    struct NodeEntry Links;
} Visitor;

typedef struct VisitorLoopParams {
    Visitor* Visitor;
    NodeEntry* listHead;
} VisitorLoopParams;

LPDWORD dwThreadId[999];
HANDLE hThreadHandles[999];
HANDLE VisitorEnterEvent;
int VisitorTID;


Visitor* AddVisitor(NodeEntry* VisitorListHead, LPTSTR Name);
DWORD WINAPI VisitorLoop(VisitorLoopParams* Params);
HANDLE InitVisitorsEvent();
DWORD WINAPI EnumVisitors(NodeEntry* VisitorListHead, BOOL PrintToConsole);
Visitor* RemoveVisitor(NodeEntry* VisitorListHead, LPTSTR Name);
DWORD WINAPI AddVisitorsThread(BOOL* go);
DWORD WINAPI GetVisitorCount(NodeEntry* VisitorListHead);
DWORD WINAPI ShowCaseAnimal(NodeEntry* VisitorListHead, int cagenum);
DWORD WINAPI EnterZoo();
DWORD WINAPI ExitZoo();

#endif