#pragma once
#include "TCharacter.h"
class TEnemy : public TCharacter
{
private:
	//
	weak_ptr<AActor> player;
	// 
	float hitElapsed = 0.0f;
	// FX
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;
	Vec3 m_vLastMoveDir;
	Vec3 velocity;
	bool m_bCrashSet = false;

	int m_iDamaged = 1;

public:
	void Tick() override;
public:
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
	const weak_ptr<AActor>& GetPlayer() { return player; }

	bool CheckHit();
	virtual void CheckHitByProjectile(int type, bool _hit);
public:
	//FX
	void Flashing();
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value);
	void ApplyCrash();
	void PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount = 5, int _maxCount = 10);
	void ApplyCrashToAllMaterials(shared_ptr<UMeshComponent> comp, bool enabled);

	float GetDamaged() const { return m_iDamaged; }
	void SetDamaged(float _damage) { m_iDamaged = _damage; }
};

