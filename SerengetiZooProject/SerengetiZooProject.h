#ifndef SERENGETI_ZOO_PROJECT_H
#define SERENGETI_ZOO_PROJECT_H

#include <Windows.h>

#define IS_LIST_EMPTY(listHead) ((listHead)->Blink == (listHead))

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

CRITICAL_SECTION VisitorListCrit;
CRITICAL_SECTION AnimalListCrit;
CRITICAL_SECTION cScore;

// Lists
NodeEntry* animalListHead;
NodeEntry* visitorListHead;

// Events
HANDLE feedEvent;

HANDLE cage1;
HANDLE cage2;
HANDLE cage3;
HANDLE cage4;
HANDLE cage5;

int g_Score;

#endif
