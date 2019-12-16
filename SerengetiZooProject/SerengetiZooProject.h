#pragma once

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;

int g_Score;
