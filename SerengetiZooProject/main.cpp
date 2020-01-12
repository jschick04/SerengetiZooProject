#include <ConsoleColors.h>
#include <cstdio>
#include <ctime>
#include <cwl.h>
#include <strsafe.h>
#include <tchar.h>
#include <wil/resource.h>
#include <Windows.h>
#include "GameManager.h"
#include "Menu.h"
#include "Zoo.h"

int _tmain() {
    srand(unsigned(time(nullptr)) * GetProcessId(GetCurrentProcess()));

    const auto zoo = wil::make_unique_nothrow<Zoo>(GameManager::MaxCages);
    THROW_LAST_ERROR_IF_NULL(zoo);

    do {
        TCHAR buffer[MAX_PATH];

        int selectedMenuNumber;
        int selectedCageNumber;

        zoo->IsOpen = true;

        Menu::PrintCurrentZooStatus();
        Menu::PrintOptions();

        _fgetts(buffer, _countof(buffer), stdin);
        if (_stscanf_s(buffer, _T("%d"), &selectedMenuNumber) != 1) {
            cwl::WriteLine(_T("Invalid Selection...\n"));
            continue;
        }

        switch (selectedMenuNumber) {
            case 1 : // Feed Animals
                cwl::WriteLine(_T("\n%cYou selected - Feed Animals\n\n"), SKYBLUE);

                zoo->GetAllAnimalsHealth();

                if (zoo->IsZooEmpty()) { break; }

                cwl::WriteLine(_T("\nWhich cage number would you like to feed?\n"));
                _fgetts(buffer, _countof(buffer), stdin);
                if (_stscanf_s(buffer, _T("%d"), &selectedCageNumber) == 1) {
                    if (selectedCageNumber < 1 || selectedCageNumber > static_cast<int>(zoo->Cages.size())) {
                        cwl::WriteLine(_T("Invalid Selection...\n"));
                    } else {
                        zoo->Cages.at(selectedCageNumber--)->FeedEvent.SetEvent();
                    }
                } else {
                    cwl::WriteLine(_T("Invalid Selection...\n"));
                }

                break;
            case 2 : // Check Animal Interactivity Levels
                cwl::WriteLine(_T("\n%cYou selected - Check Animal Interactivity Levels\n\n"), SKYBLUE);
                zoo->GetAllAnimalsInteractivity();

                break;
            case 3 : // Show Case Animal
                cwl::WriteLine(_T("\n%cYou selected - Show Case Animal\n\n"), SKYBLUE);

                cwl::WriteLine(_T("Which cage number would you like to showcase?\n"));
                _fgetts(buffer, _countof(buffer), stdin);
                if (_stscanf_s(buffer, _T("%d"), &selectedCageNumber) == 1) {
                    if (selectedCageNumber < 1 || selectedCageNumber > zoo->Cages.size()) {
                        cwl::WriteLine(_T("Invalid Selection...\n"));
                    } else {
                        //ShowCaseAnimal(visitorListHead, selectedCageNumber);
                    }
                } else {
                    cwl::WriteLine(_T("Invalid Selection...\n"));
                }

                break;
            case 4 : // Check Visitors Happiness Level
                cwl::WriteLine(_T("\n%cYou selected - Check Visitors Happiness Level\n\n"), SKYBLUE);
                //EnumVisitors(visitorListHead, TRUE);

                break;
            case 5 : // Close the zoo for the day
                zoo->EndTurn();
                break;
            case 0 :
                cwl::WriteLine(_T("\n%cYou selected - Quit\n\n"), SKYBLUE);
                zoo->EndTurn();

                GameManager::AppClose.SetEvent();

                for (auto& thread : zoo->Cages) {
                    thread->WaitForThreads();
                }

                return 0;
            default :
                cwl::WriteLine(_T("Invalid Selection...\n"));
                break;
        }

        Sleep(100);
    } while (true);
}

//
//// Move into Zoo
//HANDLE zooOpenEventThread;
//HANDLE zooOpenEventTimer = NULL;
//LARGE_INTEGER liDueTime;
//
//DWORD WINAPI mTimer(LPVOID) {
//    do {
//        HANDLE events[2];
//        events[0] = zooOpenEventTimer;
//        events[1] = appClose;
//
//        if (WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE) == 0) {
//            cwl::WriteLine(_T("\n%c------------------------------------\n"), RED);
//            cwl::WriteLine(_T("%cThe Zoo has been re-opened after the significant event.\n"), RED);
//            cwl::WriteLine(_T("%c------------------------------------\n"), RED);
//
//            IsOpen = TRUE;
//
//            EnterZoo();
//
//            PrintCurrentZooStatus();
//            PrintMenu();
//        } else {
//            return 0;
//        }
//    } while (TRUE);
//
//}
//
//// Move into Zoo constructor
//void InitializeZoo() {
//    

//
//    //BareBones Entry to test the random visitor entering simulation test.
//    HANDLE hvisitorcreationthread = NULL;
//    hvisitorcreationthread = CreateThread(
//        NULL,
//        0,
//        AddVisitorsThread,
//        NULL,
//        0,
//        NULL
//    );
//    if (hvisitorcreationthread == NULL) {
//        cwl::WriteLine(_T("Failed to create visitor creation thread"), GetLastError());
//        ExitProcess(1);
//    }
//}
