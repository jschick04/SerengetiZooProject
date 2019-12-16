#pragma once

typedef struct NodeEntry {
    struct NodeEntry* Flink;
    struct NodeEntry* Blink;
} NodeEntry;