#pragma once

#include <tchar.h>
#include <vector>
#include <wil/resource.h>
#include <Windows.h>

class Helpers {
public:
    static int GetRandomInteractiveLevel();
    static void AddRandomName(LPCTSTR name);
    static LPCTSTR GetRandomName();

private:
    static wil::critical_section m_cs;
    static std::vector<LPCTSTR> m_uniqueNames{
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
};
