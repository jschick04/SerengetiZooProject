#ifndef Visitor_H
#define Visitor_H
#include <Windows.h>
#include "Animals.h"

enum VisitorStatus { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

typedef struct VisitorNodeEntry {
    struct VisitorNodeEntry* Flink;
    struct VisitorNodeEntry* Blink;
} VisitorNodeEntry;

typedef struct Visitor {
    LPTSTR UniqueName;
    LPTSTR CageLocation;
    DWORD HappinessLevel;
    enum VisitorStatus Status;
    struct VisitorNodeEntry Links;
} Visitor;


Visitor* AddVisitor(VisitorNodeEntry* VisitorListHead, const char* Name);
DWORD WINAPI VisitorLoop(Visitor* Visitor, AnimalList* Animals);
HANDLE InitVisitorsEvent();

#endif