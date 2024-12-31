#include "szpch.h"

#include "Helpers.h"

namespace SerengetiZoo
{
    wil::critical_section Helpers::s_cs;

    // Adds a name back into the unique name list
    void Helpers::AddRandomName(const LPCTSTR name)
    {
        auto guard = s_cs.lock();

        for (auto& uniqueName : s_uniqueNames)
        {
            if (uniqueName == nullptr)
            {
                uniqueName = name;

                break;
            }
        }
    }

    // Get a random name from the unique name list
    LPCTSTR Helpers::GetRandomName()
    {
        auto guard = s_cs.lock();

        std::random_device generator;
        std::uniform_int_distribution range(0, static_cast<int>(s_uniqueNames.size()) - 1);

        int index;
        LPCTSTR selectedName;

        do
        {
            index = range(generator);
            selectedName = s_uniqueNames[index];
        }
        while (selectedName == nullptr);

        s_uniqueNames.at(index) = nullptr;

        return selectedName;
    }

    std::array<LPCTSTR, 51> Helpers::s_uniqueNames {
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
}
