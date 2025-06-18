#pragma once
#include "Singleton.h"
enum class EventType
{
	EVENT_LADDER = 0,
	EVENT_FENCE,
};
class EventManager : public Singleton<EventManager>
{
public:
	EventManager() = default;
	~EventManager();
private:
	map<wstring, function<void()>> ladderEvent;
	map<wstring, function<void()>> fenceEvent;
public:
	void AddLadderEvent(wstring _name, function<void()> _func);
	void AddFenceEvent(wstring _name, function<void()> _func);
public:
	void TriggerEvent(EventType _type, wstring _name);
};

