#pragma once
#include "AActor.h"

class TCharacter : public AActor
{
public:
	TCharacter() = default;
	virtual ~TCharacter() = default;
protected:
	int hp = 0;
	int m_iMax_Hp = 0;
	bool bDead = false;
	bool bHitByProjectile = false;

public:
	virtual void Tick() override;
	virtual void Destroy() override;
public:
	void SetHp(int _hp);
	int GetHp() { return hp; }
	void SetMaxHp(int _maxHp) { m_iMax_Hp = _maxHp; }
	int GetMaxHp() const { return m_iMax_Hp; }
	bool IsDead() { return bDead; }
public:
	void TakeDamage(int _damage);
	virtual void CheckHitByProjectile(int type, bool _hit);
	bool IsHitByProjectile() { return bHitByProjectile; }

protected:
	// 이 세계의 모든 캐릭터는 영혼을 가지고 있다...
	int m_iSoul = 10000;
public:
	int GetHisSoul() const { return m_iSoul; }
	void SetSoul(int _soul) { m_iSoul = _soul; }

};


