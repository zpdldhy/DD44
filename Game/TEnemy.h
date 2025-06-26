#pragma once
#include "TCharacter.h"
class TEnemy : public TCharacter
{
public:
	virtual ~TEnemy() = default;
private:
	//
	weak_ptr<AActor> player;
	// 
	// mage 이동 처리를 위한 변수 추가
	float hitElapsed = 0.0f;
	float hitOffset = 1.0f;
	// FX
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;
	Vec3 m_vLastMoveDir;
	Vec3 velocity;
	bool m_bCrashSet = false;
	Vec2 m_bloodRange;
	bool checkCollision = true;

	// Frustum 내부에 들어왔을 때 충돌 연산
	bool m_bFrustumIn = false;

	// 
	int stageIndex = -1;

	// 입는 데미지 연산용
	float m_iDamagedByM = 1.0f;
	float m_iDamagedByP = 1.0f;

public:
	void Tick() override;
	void Destroy() override;
public:
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
	const weak_ptr<AActor>& GetPlayer() { return player; }

	void SetHitEnable(bool _b) { checkCollision = _b; }
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

	float GetDamagedByM() const { return m_iDamagedByM; }
	void SetDamagedByM(float _damage) { m_iDamagedByM = _damage; }

	float GetDamagedByP() const { return m_iDamagedByP; }
	void SetDamagedByP(float _damage) { m_iDamagedByP = _damage; }
};

