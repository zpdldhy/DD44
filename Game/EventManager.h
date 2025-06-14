#pragma once
#include "Singleton.h"
class EventManager : public Singleton<EventManager>
{
public:
	EventManager() = default;
	~EventManager();
private:
	map<int, function<void()>> ladderEvent;
public:
	void AddLadderEvent(int _idx, function<void()> _func);
	void TriggerLadderEvent(int _idx);
};

