#ifndef Visitor_H
#define Visitor_H

#include <Windows.h>

enum VisitorStatus { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

typedef struct Visitor {
    LPCTSTR UniqueName;
    LPCTSTR CageLocation;
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

CRITICAL_SECTION VisitorListCS;
Visitor* AddVisitor(NodeEntry* VisitorListHead, LPCTSTR Name);
DWORD WINAPI VisitorLoop(LPVOID Params);
HANDLE InitVisitorsEvent();
DWORD WINAPI EnumVisitors(NodeEntry* VisitorListHead, BOOL PrintToConsole);
Visitor* RemoveVisitor(NodeEntry* VisitorListHead, LPCTSTR Name);
DWORD WINAPI AddVisitorsThread(LPVOID);
DWORD WINAPI GetVisitorCount(NodeEntry* VisitorListHead);
DWORD WINAPI ShowCaseAnimal(NodeEntry* VisitorListHead, int cagenum);
DWORD WINAPI EnterZoo();
DWORD WINAPI ExitZoo();

#endif
