#ifndef SERENGETI_ZOO_PROJECT_H
#define SERENGETI_ZOO_PROJECT_H

#include <Windows.h>

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

CRITICAL_SECTION AnimalListCrit;
CRITICAL_SECTION cScore;

// Lists
NodeEntry* animalListHead;
NodeEntry* visitorListHead;

// Events
HANDLE feedEvent;

int g_Score;

#endif
