#pragma once
#include "TCharacter.h"
#include "TriggerData.h"

class TPlayer : public TCharacter
{
public:
	virtual ~TPlayer() = default;
private:
	int arrowCount = 4;
	bool isClimbing = false;
	TriggerData m_Trigger;

	// ±‚≈∏ status
	//int 
public:
	int GetArrowCount() { return arrowCount; }
	void IncArrowCount(int _count = 1);
	void DecArrowCount(int _count = 1);
public:
	bool IsClimbing() { return isClimbing; }
	void StartClimbing();
	void StopClimbing();

	// Trigger
public:
	void ClearTrigger() { ZeroMemory(&m_Trigger, sizeof(TriggerData)); }
	void SetTrigger(TriggerData _trigger) { m_Trigger = _trigger; }
	TriggerData GetTrigger() { return m_Trigger; }
};

