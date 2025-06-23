#pragma once
#include "AActor.h"

class TCharacter : public AActor
{
public:
	virtual ~TCharacter() = default;
protected:
	int hp;
	int m_iMax_Hp;
	bool bDead = false;
	bool bHitByProjectile = false;

public:
	virtual void Tick() override;
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
	// �� ������ ��� ĳ���ʹ� ��ȥ�� ������ �ִ�...
	int m_iSoul = 0;
public:
	int GetSoul() const { return m_iSoul; }
	void SetSoul(int _soul) { m_iSoul = _soul; }

};


