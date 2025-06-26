#pragma once
#include "Singleton.h"
enum class EventType
{
	EVENT_LADDER = 0,
	EVENT_FENCE,
	EVENT_STAGE,
};
class EventManager : public Singleton<EventManager>
{
public:
	EventManager() = default;
	~EventManager();
private:
	map<wstring, function<void()>> ladderEvent;
	map<wstring, function<void()>> fenceEvent;
	map<wstring, function<void()>> stageEvent;
public:
	void AddLadderEvent(wstring _name, function<void()> _func);
	void AddFenceEvent(wstring _name, function<void()> _func);
	void AddStageEvent(wstring _name, function<void()> _func);
public:
	void TriggerEvent(EventType _type, wstring _name);
};

