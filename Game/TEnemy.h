#pragma once
#include "TCharacter.h"
class TEnemy : public TCharacter
{
private:
	//
	weak_ptr<AActor> player;
	// 
	float hitElapsed = 0.0f;
	float hitOffset = 1.0f;
	// FX
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;
	Vec3 m_vLastMoveDir;
	Vec3 velocity;
	bool m_bCrashSet = false;
	Vec2 m_bloodRange;

	// Frustum 내부에 들어왔을 때 충돌 연산
	bool m_bFrustumIn = false;

	// 
	int stageIndex = -1;

	// 입는 데미지 연산용
	int m_iDamaged = 1;

public:
	void Tick() override;
	void Destroy() override;
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

	void SetBloodPos(const Vec2& _range) { m_bloodRange = _range; }
	void SetHitOffset(float _offset) { hitElapsed = _offset; }
	void SetStageId(UINT _id) { stageIndex = _id; }

	void SetFrustumIn(bool _isFrustumIn) { m_bFrustumIn = _isFrustumIn; }
	bool IsFrustumIn() { return m_bFrustumIn; }

	float GetDamaged() const { return m_iDamaged; }
	void SetDamaged(float _damage) { m_iDamaged = _damage; }
};

