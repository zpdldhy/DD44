#pragma once
#include "UScriptComponent.h"
#include "PlayerStates.h"

class APawn;
class AActor;
class UAnimInstance;
class UMeshComponent;

class PlayerMoveScript : public UScriptComponent
{
public:
	PlayerMoveScript() = default;
	virtual ~PlayerMoveScript() = default;
public:
	float m_fSpeed  = 13.0f; 
	float m_fRotationSpeed = 8.0f;
	Vec3 m_vLook;
	Vec3 m_vRight;

	// ANIMATION
	shared_ptr<UAnimInstance> m_pAnimInstance;
	bool m_bAttack = false;

	// Child Mesh Components
	// 어떻게 관리해야 좋을지 모르겟음
	weak_ptr<UMeshComponent> sword;
	weak_ptr<UMeshComponent> effect;
	weak_ptr<UMeshComponent> handSocket;
	weak_ptr<UMeshComponent> backSocket;

	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> attack;
	shared_ptr<StateBase> currentState;

public:
	void Init() override;
	void Tick() override;
public:
	void ChangetState(shared_ptr<StateBase> _state);
};

