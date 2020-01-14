#include "Visitor.h"

#include <tchar.h>
#include "Helpers.h"

wil::critical_section Visitor::CriticalSection;

Visitor::Visitor() {
    UniqueName = Helpers::GetRandomName();
    CageLocation = _T("Entry");
    HappinessLevel = 8;
    Status = VisitorStatus::Happy;

    // TODO: Call Visitor Loop thread
}

int Visitor::GetVisitorCount() {
    // TODO: Finish GetVisitorCount
    return 0;
}

void Visitor::RemoveVisitor(LPCTSTR name) {
    auto guard = CriticalSection.lock();

    // TODO: Finish RemoveVisitor
    //{
    //    WaitForSingleObject(hVisitorEvent, INFINITE);
    //    EnterCriticalSection(&VisitorListCrit);
    //
    //    NodeEntry* RemovedNode = static_cast<NodeEntry*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry)));
    //    if (RemovedNode == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    NodeEntry* TempNodePrev = static_cast<NodeEntry*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry)));
    //    if (TempNodePrev == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    NodeEntry* TempNodeNext = static_cast<NodeEntry*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NodeEntry)));;
    //    if (TempNodeNext == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    RemovedNode = VisitorListHead->Flink;
    //    Visitor* RemovedVisitor = static_cast<Visitor*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor)));
    //    if (RemovedVisitor == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    Visitor* PreviousVisitor = static_cast<Visitor*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor)));
    //    if (PreviousVisitor == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //    Visitor* NextVisitor = static_cast<Visitor*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Visitor)));
    //    if (NextVisitor == NULL) {
    //        cwl::WriteLine(_T("%cFailed to allocate memory\n"), RED, GetLastError());
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //        return NULL;
    //    }
    //
    //    //loop through all to find a matching uniquename.
    //    while (RemovedVisitor->UniqueName != Name)
    //    {
    //        RemovedVisitor = CONTAINING_RECORD(RemovedNode, Visitor, Links);
    //        RemovedNode = RemovedNode->Flink;
    //    }
    //    //when a unique name is found we need to update the flink of the previous node and the blink of the next node.
    //    TempNodePrev = RemovedVisitor->Links.Blink;
    //    PreviousVisitor = CONTAINING_RECORD(TempNodePrev, Visitor, Links);
    //    TempNodeNext = RemovedVisitor->Links.Flink;
    //    NextVisitor = CONTAINING_RECORD(TempNodeNext, Visitor, Links);
    //    PreviousVisitor->Links.Flink = TempNodeNext;
    //    NextVisitor->Links.Blink = TempNodePrev;
    //
    //    //cleanup
    //    HeapFree(GetProcessHeap(), 0, RemovedVisitor);
    //
    //    LeaveCriticalSection(&VisitorListCrit);
    //    SetEvent(hVisitorEvent);
    //
    //    return 0;
    //}
}

DWORD WINAPI Visitor::VisitorLoopThread(LPVOID lpParam) {
    // TODO: Finish VisitorLoopThread
    //auto Params = static_cast<VisitorLoopParams*>(Param);
    //int SleepTimeRand = 0;
    //for (int i = 0; i != _countof(cages); ++i) {
    //    //sleep between 1 and 3 minutes to simulate walk/watch times. 
    //    int sleeparray[5];
    //    for (int s = 0; s != 5; ++s) {
    //        sleeparray[s] = SleepTimeRand = (rand() % (1800 - 600)) + 600;
    //    }
    //    int selector = (rand() % 5);

    //    //sleep while waking to check if exit zoo is true.
    //    for (int t = 0; t != 10; ++t) {
    //        if (bExitZoo == TRUE) {
    //            //LeaveCriticalSection(&VisitorListCrit);
    //            break;
    //        } else {
    //            Sleep(sleeparray[selector]);
    //        }

    //    }

    //    //handle error if the cage name is NULL. Something is very wrong, there are no animals.
    //    if (IsCageEmpty(cages[i]->Name)) {
    //        //WriteConsoleOutput(_T("There are no animals in the zoo left!"),RED);
    //         //cwl::WriteLine(_T("There are no animals left in the Zoo!\n"));
    //    } else {
    //        WaitForSingleObject(hVisitorEvent, INFINITE);
    //        EnterCriticalSection(&VisitorListCrit);

    //        Params->Visitor->CageLocation = cages[i]->Name;
    //        cwl::WriteLine(_T("%s entered cage: %s\n"), Params->Visitor->UniqueName, Params->Visitor->CageLocation);

    //        if (IsCageEmpty(Params->Visitor->CageLocation) == TRUE) {
    //            Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel - 2;
    //            cwl::WriteLine(_T("%s lost 2 happiness points after visiting %s due to being empty!\n"), Params->Visitor->UniqueName, Params->Visitor->CageLocation);
    //        } else {
    //            //Get the interactivity level and increase or decrease happiness of visitors
    //            DWORD AverageInterActivityLevel = 0;
    //            AverageInterActivityLevel = GetCageTotalInteractiveLevel(cages[i]->Name);

    //            //increase or decrease happiness point based on interctivity level
    //            if (AverageInterActivityLevel <= 4) {
    //                if (Params->Visitor->HappinessLevel != 0) {
    //                    Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel - 1;
    //                    cwl::WriteLine(_T("%s %clost%r a happiness point after visiting %s!\n"), Params->Visitor->UniqueName, RED, Params->Visitor->CageLocation);
    //                }

    //            } else if (AverageInterActivityLevel == 5) {
    //                Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel;
    //            } else {
    //                if (Params->Visitor->HappinessLevel != 10) {
    //                    Params->Visitor->HappinessLevel = Params->Visitor->HappinessLevel + 1;
    //                    cwl::WriteLine(_T("%s %cgained%r a happiness point after visiting %s!\n"), Params->Visitor->UniqueName, GREEN, Params->Visitor->CageLocation);
    //                }
    //            }
    //        }

    //        //after the increase or decrease update status of visitor.
    //        if (Params->Visitor->HappinessLevel < 5) {
    //            Params->Visitor->Status = RefundDemanded;
    //            LeaveCriticalSection(&VisitorListCrit);
    //            SetEvent(hVisitorEvent);
    //            break;

    //        }
    //        if (Params->Visitor->HappinessLevel > 5 && Params->Visitor->HappinessLevel <= 7) {
    //            Params->Visitor->Status = Disappointed;

    //        }
    //        if (Params->Visitor->HappinessLevel >= 8) {
    //            Params->Visitor->Status = Happy;
    //        }

    //        if (bExitZoo == TRUE) {
    //            LeaveCriticalSection(&VisitorListCrit);
    //            SetEvent(hVisitorEvent);
    //            break;
    //        }
    //        LeaveCriticalSection(&VisitorListCrit);
    //        SetEvent(hVisitorEvent);
    //    }
    //}

    //Sleep(SleepTimeRand);
    ////visitor should have visited all locations at this point so we need to calculate if leaving happy leaving angry or demanding a refund.
    //LPCTSTR ExitStatus;
    //WaitForSingleObject(hVisitorEvent, INFINITE);
    //EnterCriticalSection(&VisitorListCrit);

    //if (Params->Visitor->HappinessLevel < 5) {
    //    Params->Visitor->HappinessLevel = RefundDemanded;
    //    ExitStatus = _T("Demanded a Refund");
    //    cwl::WriteLine(_T("%s %c%s\n"), Params->Visitor->UniqueName, RED, ExitStatus);
    //} else if (Params->Visitor->HappinessLevel > 5 && Params->Visitor->HappinessLevel <= 7) {
    //    Params->Visitor->HappinessLevel = LeavingAngry;
    //    ExitStatus = _T("Left Angry");
    //    cwl::WriteLine(_T("%s %c%s\n"), Params->Visitor->UniqueName, RED, ExitStatus);
    //    g_Score = g_Score - 10;
    //} else {
    //    Params->Visitor->HappinessLevel = LeavingHappy;
    //    ExitStatus = _T("Left Happy");
    //    cwl::WriteLine(_T("%s %c%s\n"), Params->Visitor->UniqueName, GREEN, ExitStatus);
    //    g_Score = g_Score + 10;

    //}

    //LeaveCriticalSection(&VisitorListCrit);
    //SetEvent(hVisitorEvent);
    //Sleep(1000);
    //RemoveVisitor(visitorListHead, Params->Visitor->UniqueName);

    return 0;
}
