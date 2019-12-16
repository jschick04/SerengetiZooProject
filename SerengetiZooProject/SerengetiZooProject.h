#ifndef SERENGETI_ZOO_PROJECT_H
#define SERENGETI_ZOO_PROJECT_H

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

CRITICAL_SECTION AnimalListCrit;
CRITICAL_SECTION cScore;

NodeEntry* animalListHead;
NodeEntry* visitorListHead;

int g_Score;

#endif
