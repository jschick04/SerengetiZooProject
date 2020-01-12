#include "GameManager.h"

int GameManager::SignificantEventMinutes = 2;
int GameManager::FeedEventMinutes = 1;

int GameManager::FeedLevelMax = 4;
int GameManager::FeedLevelMin = 2;

int GameManager::HungerLevel = 1;

int GameManager::MaxCages = 6;

int GameManager::Score = 0;

wil::unique_event_failfast GameManager::AppClose(wil::EventOptions::ManualReset);
