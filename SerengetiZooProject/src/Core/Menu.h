#pragma once

#include "Entities/Zoo.h"

namespace SerengetiZoo::Menu
{
    inline void PrintCurrentZooStatus()
    {
        cwl::WriteLine(_T("\n%c-------------------------\n"), YELLOW);
        cwl::WriteLine(_T("%cZoo Status: "), YELLOW);

        if (Zoo::IsOpen())
        {
            cwl::WriteLine(_T("%cOpen\n"), LIME);
        }
        else
        {
            cwl::WriteLine(_T("%cClosed\n"), PINK);
        }

        cwl::WriteLine(_T("%c-------------------------\n"), YELLOW);
    };

    inline void PrintOptions()
    {
        cwl::WriteLine(_T("%cPlease select your action\n"), LIME);
        cwl::WriteLine(_T("%c-------------------------\n"), YELLOW);
        cwl::WriteLine(_T("%c1%r - Feed Animals\n"), LIME);
        cwl::WriteLine(_T("%c2%r - Check Animal Interactivity Levels\n"), LIME);
        cwl::WriteLine(_T("%c3%r - Show Case Animal\n"), LIME);
        cwl::WriteLine(_T("%c4%r - Check Visitors Happiness Level\n"), LIME);
        cwl::WriteLine(_T("%c5%r - Close zoo for the day\n"), LIME);
        cwl::WriteLine(_T("%c0%r - Exit\n"), PINK);
        cwl::WriteLine(_T("%c-------------------------\n"), YELLOW);
    };

    inline void PrintScore()
    {
        /*cwl::WriteLine(_T("\n%c-------------------------\n"), YELLOW);
        cwl::WriteLine(_T("%c Score = %d\n"), YELLOW, GameManager::Score);
        cwl::WriteLine(_T("%c-------------------------\n"), YELLOW);*/
    };
};
