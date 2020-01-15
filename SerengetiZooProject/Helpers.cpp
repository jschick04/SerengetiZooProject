#include "Helpers.h"

#include <random>
#include <tchar.h>
#include "Animal.h"
#include "Visitor.h"

wil::critical_section Helpers::m_cs;

// Adds a name back into the unique name list
void Helpers::AddRandomName(LPCTSTR name) {
    auto guard = m_cs.lock();

    for (int i = 0; i != m_uniqueNames.size(); ++i) {
        if (m_uniqueNames[i] == nullptr) {
            m_uniqueNames[i] = name;
            break;
        }
    }
}

// Get a random name from the unique name list
LPCTSTR Helpers::GetRandomName() {
    auto guard = m_cs.lock();

    std::random_device generator;
    const std::uniform_int_distribution<int> range(0, int(m_uniqueNames.size()) - 1);

    int index;
    LPCTSTR selectedName;

    do {
        index = range(generator);
        selectedName = m_uniqueNames[index];
    } while (selectedName == nullptr);

    m_uniqueNames.at(index) = nullptr;

    return selectedName;
}

LPCTSTR Helpers::AnimalTypeToString(const AnimalType type) {
    switch (type) {
        case AnimalType::Antelope :
            return _T("Antelope");
        case AnimalType::Cheetah :
            return _T("Cheetah");
        case AnimalType::Giraffe :
            return _T("Giraffe");
        case AnimalType::Hyaena :
            return _T("Hyaena");
        case AnimalType::Hippo :
            return _T("Hippo");
        case AnimalType::Monkey :
            return _T("Monkey");
        case AnimalType::Mongoose :
            return _T("Mongoose");
        case AnimalType::Tiger :
            return _T("Tiger");
        case AnimalType::WildeBeast :
            return _T("WildeBeast");
        case AnimalType::Zebra :
            return _T("Zebra");
        default :
            return _T("Invalid Animal");
    }
}

LPCTSTR Helpers::VisitorStatusToString(const VisitorStatus type) {
    switch (type) {
        case VisitorStatus::Disappointed :
            return _T("Disappointed");
        case VisitorStatus::Happy :
            return _T("Happy");
        case VisitorStatus::LeavingAngry :
            return _T("leaving angry");
        case VisitorStatus::LeavingHappy :
            return _T("happily leaving the zoo");
        case VisitorStatus::RefundDemanded :
            return _T("demanding a refund");
        default :
            return _T("Invalid Status");
    }
}

std::array<LPCTSTR, 51> Helpers::m_uniqueNames{
    _T("Julien"),
    _T("Melman"),
    _T("Maurice"),
    _T("Gloria"),
    _T("Mason"),
    _T("Ron"),
    _T("Deveroux"),
    _T("Felicio"),
    _T("Gary"),
    _T("Jorge"),
    _T("Joseph"),
    _T("Travis"),
    _T("Ryan"),
    _T("Tony"),
    _T("Leonardo"),
    _T("Barry"),
    _T("David"),
    _T("Bruce"),
    _T("Cliff"),
    _T("Jack"),
    _T("Emma"),
    _T("Taylor"),
    _T("Alex"),
    _T("Sophia"),
    _T("James"),
    _T("Diego"),
    _T("Samuel"),
    _T("Maria"),
    _T("Richard"),
    _T("Logan"),
    _T("Brianna"),
    _T("Martin"),
    _T("Jessica"),
    _T("Patricia"),
    _T("Linda"),
    _T("Caroline"),
    _T("Liam"),
    _T("Oliver"),
    _T("Ben"),
    _T("Dorothy"),
    _T("Mabel"),
    _T("Ariel"),
    _T("Louis"),
    _T("Nathan"),
    _T("Nicole"),
    _T("Adam"),
    _T("Felix"),
    _T("Jon"),
    _T("Marco"),
    _T("Harry"),
    _T("Anna")
};
