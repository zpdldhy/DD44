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
public:
	void Tick() override;
public:
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
	const weak_ptr<AActor>& GetPlayer() { return player; }

	bool CheckHit();
public:
	//FX
	void Flashing();
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value);
	void PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount = 5, int _maxCount = 10);
};

