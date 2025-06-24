#pragma once
#include "TCharacter.h"
#include "TriggerData.h"

enum class EStatType
{
	HP = 0,
	MeleeDamage,
	MoveSpeed,
	RangedDamage,
	COUNT
};

struct StatUpgradeInfo
{
	int soulCost;
	float delta1;
	float delta2;
};

class TPlayer : public TCharacter
{
public:
	virtual ~TPlayer() = default;
private:
	int arrowCount = 4;
	bool isClimbing = false;
	TriggerData m_Trigger;

	// 스테이터스
	float m_fMaxHP = 4.f;
	float m_fMeleeDamage = 1.f;
	Vec3 m_vMeleeRange = Vec3(3.f, 1.f, 2.f);
	float m_fMoveSpeed = 0.25f;
	float m_fRollSpeed = 0.5f;
	float m_fRangedDamage = 1.f;
	float m_fArrowCount = 4.f;
	
	// 각 능력치 현재 레벨
	int m_iStatLevel[(int)EStatType::COUNT] = { 0 };

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
public:
	// 영혼(재화)
	void AddSoul(int _amount) { m_iSoul += _amount; }
	bool SpendSoul(int _amount);

	// 스테이터스
	int GetStatLevel(EStatType type) const { return m_iStatLevel[(int)type]; }
	bool UpgradeStat(EStatType type);

	// MaxHP
	float GetMaxHP() const { return m_fMaxHP; }
	void  SetMaxHP(float hp) { m_fMaxHP = hp; }

	// Melee Damage
	float GetMeleeDamage() const { return m_fMeleeDamage; }
	void  SetMeleeDamage(float dmg) { m_fMeleeDamage = dmg; }

	// Melee Range
	Vec3 GetMeleeRange() const { return m_vMeleeRange; }
	void SetMeleeRange(const Vec3& range) { m_vMeleeRange = range; }

	// Move Speed
	float GetMoveSpeed() const { return m_fMoveSpeed; }
	void  SetMoveSpeed(float speed) { m_fMoveSpeed = speed; }

	// Roll Speed
	float GetRollSpeed() const { return m_fRollSpeed; }
	void  SetRollSpeed(float speed) { m_fRollSpeed = speed; }

	// Ranged Damage
	float GetRangedDamage() const { return m_fRangedDamage; }
	void  SetRangedDamage(float dmg) { m_fRangedDamage = dmg; }

	// Arrow Count Max
	float GetArrowCapacity() const { return m_fArrowCount; }
	void  SetArrowCapacity(float count) { m_fArrowCount = count; }

};

