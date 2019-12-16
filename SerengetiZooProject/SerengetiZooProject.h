#pragma once

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

CRITICAL_SECTION AnimalListCrit;
CRITICAL_SECTION cScore;

NodeEntry* animalListHead;
NodeEntry* visitorListHead;

int g_Score;
