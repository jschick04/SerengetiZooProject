#pragma once
#include <tchar.h>

namespace SerengetiZoo
{
    enum class VisitorStatus : uint8_t { Happy, Disappointed, RefundDemanded, LeavingHappy, LeavingAngry };

    inline LPCTSTR ToString(const VisitorStatus status)
    {
        switch (status)
        {
            case VisitorStatus::Happy: return _T("Happy");
            case VisitorStatus::Disappointed: return _T("Disappointed");
            case VisitorStatus::RefundDemanded: return _T("Refund Demanded"); 
            case VisitorStatus::LeavingHappy: return _T("Leaving Happy");
            case VisitorStatus::LeavingAngry: return _T("Leaving Angry");
        }

        return _T("Unknown");
    }

    class Visitor
    {
    public:
        explicit Visitor();

        Visitor(const Visitor& other) = delete;
        Visitor(Visitor&& other) = delete;
        Visitor& operator=(const Visitor& other) = delete;
        Visitor& operator=(Visitor&& other) = delete;

        bool operator==(const Visitor& other) const
        {
            return m_name == other.m_name;
        }

        bool operator!=(const Visitor& other) const
        {
            return !(*this == other);
        }

        ~Visitor()
        {
            //cwl::WriteLine(_T("%c%s has left the Zoo\n"), PINK, m_name);
        }

    public:
        void AddHappiness();
        void CalculateScore() noexcept;
        void UpdateStatus() noexcept;

        [[nodiscard]] LPCTSTR GetName() const noexcept { return m_name; }

        [[nodiscard]] DWORD GetCurrentCage() const noexcept { return m_currentCageNumber; }

        [[nodiscard]] DWORD GetHappiness() const noexcept { return m_happiness; }

        [[nodiscard]] LPCTSTR GetStatus() const noexcept { return ToString(m_status); }

        void WaitForThreads() const noexcept;

    private:
        static DWORD WINAPI VisitorLoop(LPVOID lpParam);

    private:
        void ResetMovementTimer();

    private:
        wil::critical_section m_cs;

        DWORD m_currentCageNumber;

        LPCTSTR m_name;
        LPCTSTR m_location;
        DWORD m_happiness;
        VisitorStatus m_status;

    private:
        LARGE_INTEGER m_movementTime;
        wil::unique_handle m_movementTimer;
        wil::unique_handle m_visitorLoopThread;
    };
}
