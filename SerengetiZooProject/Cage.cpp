#include "Cage.h"

Cage::Cage(LPCTSTR name) {
    Name = name;

    m_feedEvent.create(wil::EventOptions::Signaled);
}
