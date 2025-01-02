#pragma once

#include "Cage.h"
#include "Visitor.h"

namespace SerengetiZoo
{
    class Zoo
    {
    public:
        static wistd::unique_ptr<Zoo> Initialize();

        static wil::unique_event_failfast Close;
        static wil::unique_handle Open;

        static void FeedAnimals(int cageNumber);
        static void GetAllAnimals();
        static void GetAllAnimalsHealth();
        static void GetAllAnimalsInteractivity();
        static Cage* GetRandomCage();

        static std::vector<Cage>& GetCages() { return s_cages; }

        static BOOL IsOpen() { return s_isOpen; }

        static bool IsZooEmpty();

    public:
        void EndTurn();

        void GetAllVisitors() const;

        void ShowCaseAnimals(int cageNumber);

        void WaitForThreads() const;

    private:
        static DWORD WINAPI OpenZooTimerThread(LPVOID);
        static DWORD WINAPI AddVisitorsThread(LPVOID lpParam);

    private:
        explicit Zoo();

        void RemoveVisitor(const wistd::unique_ptr<Visitor>& visitor);

        void ResetAddVisitorsEvent();
        void ResetOpenZooTimer();

    private:
        static std::vector<Cage> s_cages;
        static BOOL s_isOpen;

    private:
        wil::critical_section m_cs;

        std::vector <wistd::unique_ptr<Visitor>> m_visitors;

    private:
        LARGE_INTEGER m_addVisitorsEventTime;
        wil::unique_event_failfast m_canAddNewVisitorsEvent;
        LARGE_INTEGER m_closedEventTime;
        wil::unique_handle m_newVisitorsTimer;
        wistd::unique_ptr<SignificantEvent> m_significantEvent;

    private:
        wil::unique_handle m_addVisitorsThread;
        wil::unique_handle m_zooTimerThread;
    };
}
