#include "pch.h"
#include "EventManager.h"

EventManager::~EventManager()
{
}

void EventManager::AddLadderEvent(int _idx, function<void()> _func)
{
	ladderEvent.insert(make_pair(_idx, _func));
}

void EventManager::TriggerLadderEvent(int _idx)
{
	auto event = ladderEvent.find(_idx);
	if (event != ladderEvent.end())
	{
		event->second();
	}
}
