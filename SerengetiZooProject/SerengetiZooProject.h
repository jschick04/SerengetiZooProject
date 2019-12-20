#ifndef SERENGETI_ZOO_PROJECT_H
#define SERENGETI_ZOO_PROJECT_H

#include <Windows.h>

#define IS_LIST_EMPTY(listHead) ((listHead)->Blink == (listHead))

#define TIMER_SECONDS 10000000

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

CRITICAL_SECTION AnimalListCrit;
CRITICAL_SECTION VisitorListCrit;
CRITICAL_SECTION ConsoleCrit;
CRITICAL_SECTION cScore;

HANDLE appClose;

// Lists
NodeEntry* animalListHead;
NodeEntry* visitorListHead;

int g_Score;

void EndTurnActions();

#endif
