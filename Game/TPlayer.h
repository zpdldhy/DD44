#pragma once
#include "TCharacter.h"

enum class ETriggerType
{
	TT_NONE,
	TT_LADDER,
	TT_HEALPOINT,	// 먹었을 때 아니라, 먹는 곳 앞에 서 있을 때
	TT_COUNT,
};

struct TriggerData
{
	ETriggerType eTriggerType = ETriggerType::TT_NONE;
	Vec3 vPoint = { 0.f, 0.f, 0.f };	// 어디서 일어나는지
};

class TPlayer : public TCharacter
{
public:
	virtual ~TPlayer() = default;
private:
	int arrowCount = 4;
	bool isClimbing = false;
	TriggerData m_Trigger;

	// 기타 status
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

