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


Visitor* AddVisitor(NodeEntry* VisitorListHead, const char* Name);
DWORD WINAPI VisitorLoop(Visitor* Visitor, AnimalList* Animals);
HANDLE InitVisitorsEvent();

#endif