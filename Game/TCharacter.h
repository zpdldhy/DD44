#pragma once
#include "AActor.h"

class TCharacter : public AActor
{
public:
	virtual ~TCharacter() = default;
private:
	int hp;
	bool bDead = false;
	bool bHitByProjectile = false;
public:
	virtual void Tick() override;
public:
	void SetHp(int _hp) { hp = _hp; }
	int GetHp() { return hp; }
	bool IsDead() { return bDead; }
public:
	void TakeDamage(int _damage);
	void CheckHitByProjectile(bool _hit) { bHitByProjectile = _hit; }
	bool IsHitByProjectile() { return bHitByProjectile; }
};


