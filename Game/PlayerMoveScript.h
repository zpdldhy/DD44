#pragma once
#include "UScriptComponent.h"
#include "PlayerStates.h"

class APawn;
class AActor;
class UMeshComponent;

class PlayerMoveScript : public UScriptComponent
{
public:
	PlayerMoveScript() = default;
	virtual ~PlayerMoveScript() = default;
public:
	float m_fSpeed = 10.0f;
	float m_fRollSpeed  = 15.0f;
	float m_fCurrentSpeed = 0.f;
	float m_fRotationSpeed = 8.0f;

	// Attack
	shared_ptr<AActor> attackRangeActor;
	shared_ptr<AActor> bow;
	shared_ptr<AActor> sword;
	Vec3 colOffset;

	Vec3 m_vLook;
	Vec3 m_vRight;
	Vec3 m_vLastMoveDir;
	Vec3 velocity;
	Vec3 m_vRollLook;

	shared_ptr<Texture> m_pSubTexture;
	shared_ptr<Texture> m_pNoiesTexture;

	shared_ptr<PlayerBaseState> empty;
	shared_ptr<PlayerBaseState> idle;
	shared_ptr<PlayerBaseState> walk;
	shared_ptr<PlayerBaseState> climb;
	shared_ptr<PlayerBaseState> roll;
	shared_ptr<PlayerBaseState> attack;
	shared_ptr<PlayerBaseState> hit;
	shared_ptr<PlayerBaseState> shoot;
	shared_ptr<PlayerBaseState> die;
	shared_ptr<PlayerBaseState> currentState;
	PLAYER_STATE currentStateId;

	// Cilmb
	bool m_bCanClimb = false;
	Vec3 m_vLadder;
	Vec3 m_vDirToLadder;
	Vec3 m_vLadderEnd;

	// state로 이관
	//// Slash 
	//shared_ptr<UMaterial> m_pSlashMaterial = nullptr;
	//float m_fSlashTime = 0.0f;
	//bool m_bSlashPlaying = false;
	//float m_fSlashDuration = 0.3f;
	 
	// Camera Offset
	Vec3 m_vCameraOffset = { 20.0f, 30.0f, -20.0f };
	// temp
	int animIndex = 0;

	//Crash
	bool m_bCrashSet = false;

	//Dissolve
	float m_fDissolveTimer = 0.0f;
	bool m_bDissolving = false;

	// Combo
	int maxComboCount = 3;

	// 행동 제약(Move, Attack 등)
	bool m_bNoInput = false;
	bool isSetEnd = false;

public:
	void Init() override;
	void Tick() override;
	virtual shared_ptr<UScriptComponent> Clone() override;
public:
	void Resurrection();
	void ChangeState(shared_ptr<PlayerBaseState> _state);
public:
	void PlayFX();
	//void Slash();
	void PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount = 5, int _maxCount = 10);
	void VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp);
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> _comp, float _value);
	void ApplyCrashToAllMaterials(shared_ptr<UMeshComponent> _comp, bool _enabled);
	void ApplyDissolveToAllMaterials(shared_ptr<UMeshComponent> _comp, float _time);
	void ApplyCrash();

public:
	void Move();
	void Climb();
	void CheckCollision();
	void RollMove();
	bool CanAttack();
	void UpdateCollider();
	void UpdateBow();
	void CheckCoolTIme();

	// 행동 제약
	void NoInput() { m_bNoInput = true; }
	void CanInput() { m_bNoInput = false; }
	void WalkAnim() { ChangeState(walk); }
	void IdleAnim() { ChangeState(idle); }
	void EndGame();

public:
	void CheckClimb();
	void CheckRoll();
	void CheckAttack();
	void CheckComboAttack();
	void CheckMove();
};

