#pragma once

#include "Cage.h"
#include "Visitor.h"
#include "Core/SignificantEvent.h"

namespace SerengetiZoo
{
    class Zoo
    {
    public:
        static wistd::unique_ptr<Zoo> Initialize();

        static BOOL IsOpen() { return s_isOpen; }

    public:
        static inline wil::unique_event_failfast Close = wil::unique_event_failfast(wil::EventOptions::ManualReset);
        static inline wil::unique_handle Open = wil::unique_handle(CreateWaitableTimer(nullptr, false, nullptr));

    public:
        void EndTurn();
        bool IsZooEmpty() const;

        void GetAllVisitors() const;

        [[nodiscard]] std::vector<wistd::unique_ptr<Cage>>& GetCages() { return m_cages; }

        void GetAllAnimals() const;
        void GetAllAnimalsHealth() const;
        void GetAllAnimalsInteractivity() const;
        Cage* GetRandomCage() const;

        void FeedAnimals(int cageNumber) const;
        void ShowCaseAnimals(int cageNumber);

        void WaitForThreads() const;

    private:
        static DWORD WINAPI OpenZooTimerThread(LPVOID);
        static DWORD WINAPI AddVisitorsThread(LPVOID lpParam);

    private:
        explicit Zoo();

        void OnSignificantEvent();

        void RemoveVisitor(const wistd::unique_ptr<Visitor>& visitor);

        void ResetAddVisitorsEvent();
        void ResetOpenZooTimer();

    private:
        static BOOL s_isOpen;

    private:
        wil::critical_section m_cs;

        std::vector<wistd::unique_ptr<Cage>> m_cages;
        std::vector<wistd::unique_ptr<Visitor>> m_visitors;

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
