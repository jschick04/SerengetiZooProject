#pragma once
#include <wil/resource.h>

constexpr auto TIMER_SECONDS = 10000000;

class GameManager {
public:
    static int SignificantEventMinutes;
    static int FeedEventMinutes;

    static int FeedLevelMax;
    static int FeedLevelMin;

    static int HungerLevel;

    static int MaxCages;

    static int Score;

    static wil::unique_event appClose;
};