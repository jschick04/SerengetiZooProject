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


Visitor* AddVisitor(NodeEntry* VisitorListHead, LPTSTR Name);
DWORD WINAPI VisitorLoop(Visitor* Visitor, AnimalList* Animals);
HANDLE InitVisitorsEvent();
DWORD WINAPI EnumVisitors(NodeEntry* VisitorListHead, BOOL PrintToConsole);
Visitor* RemoveVisitor(NodeEntry* VisitorListHead, LPTSTR Name);

#endif