#pragma once
#include "UScriptComponent.h"

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

	// Player's Child Mesh Components
	shared_ptr<UMeshComponent> sword;
	shared_ptr<UMeshComponent> effect;
	shared_ptr<UMeshComponent> sword_socket1;
	shared_ptr<UMeshComponent> sword_socket2;
	shared_ptr<UMeshComponent> back_socket;

public:
	void Init() override;
	void Tick() override;
};

