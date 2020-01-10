#pragma once

constexpr auto TIMER_SECONDS = 10000000;

class GameManager {
public:
    static int SignificantEventMinutes = 2;
    static int FeedEventMinutes = 1;

    static int FeedLevelMax = 4;
    static int FeedLevelMin = 2;

    static int HungerLevel = 1;

    static int MaxCages = 6;

    static int Score = 0;
};
