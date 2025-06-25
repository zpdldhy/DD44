#include "pch.h"
#include "EventManager.h"

EventManager::~EventManager()
{
}

void EventManager::AddLadderEvent(wstring _name, function<void()> _func)
{
	ladderEvent.insert(make_pair(_name, _func));
}

void EventManager::AddFenceEvent(wstring _name, function<void()> _func)
{
	fenceEvent.insert(make_pair(_name, _func));
}

void EventManager::AddStageEvent(wstring _name, function<void()> _func)
{
	stageEvent.insert(make_pair(_name, _func));
}

void EventManager::TriggerEvent(EventType _type, wstring _name)
{
	switch (_type)
	{
	case EventType::EVENT_LADDER:
	{
		auto event = ladderEvent.find(_name);
		if (event != ladderEvent.end())
		{
			event->second();
		}
	}
	break;
	case EventType::EVENT_FENCE:
	{
		auto event = fenceEvent.find(_name);
		if (event != fenceEvent.end())
		{
			event->second();
		}
	}
	break;
	case EventType::EVENT_STAGE:
	{
		auto event = stageEvent.find(_name);
		if (event != stageEvent.end())
		{
			event->second();
		}
	}
	break;
	default:
		break;
	}

}