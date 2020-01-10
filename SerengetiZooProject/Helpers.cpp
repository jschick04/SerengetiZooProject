#include "Helpers.h"

void Helpers::AddRandomName(LPCTSTR name) {
    EnterCriticalSection(&NameListCrit);

    for (int i = 0; i != _countof(uniqueNames); ++i) {
        if (uniqueNames[i] == NULL) {
            uniqueNames[i] = name;
            break;
        }
    }

    LeaveCriticalSection(&NameListCrit);
}

LPCTSTR Helpers::GetRandomName() {
    auto lock = m_cs.lock();

    unsigned int index;
    LPCTSTR selectedName;

    do {
        index = rand() % _countof(uniqueNames);
        selectedName = uniqueNames[index];
    } while (selectedName == nullptr);

    uniqueNames[index] = nullptr;

    return selectedName;
}
