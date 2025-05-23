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

	// Child Mesh Components
	// 어떻게 관리해야 좋을지 모르겟음
	weak_ptr<UMeshComponent> sword;
	weak_ptr<UMeshComponent> effect;
	weak_ptr<UMeshComponent> sword_socket1;
	weak_ptr<UMeshComponent> sword_socket2;
	weak_ptr<UMeshComponent> back_socket;

public:
	void Init() override;
	void Tick() override;
};

