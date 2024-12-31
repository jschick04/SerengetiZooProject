#pragma once

inline constexpr int TimerSeconds = 10'000'000; // 1 second (100 nanosecond intervals)

namespace SerengetiZoo::GameSettings
{
    inline constexpr int FeedEventMinutes = 1;
    inline constexpr int FeedLevelMax = 4;
    inline constexpr int FeedLevelMin = 2;

    inline constexpr int HungerLevel = 1;

    inline constexpr int MaxCages = 6;

    inline constexpr int SignificantEventMinutes = 2;
}
