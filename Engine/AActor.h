#pragma once
#include "UObject.h"
#include "UMeshComponent.h"
#include "UCameraComponent.h"
#include "UShapeComponent.h"
#include "ULightComponent.h"
#include "UPhysicsComponent.h"
#include "ConstantBuffer.h"

enum class ComponentType
{
	CT_CAMERA,
	CT_SHAPE,
	//CT_EFFECT,
	CT_MESH,
	CT_LIGHT,
	CT_COUNT,
};

enum class ActorType
{
	AT_NONE,
	AT_CHARACTER,
	AT_MONSTER,
	AT_OBJECT,
	AT_GROUND,
	AT_STAIR,
	AT_PROJECTILE,
	AT_INTERACTABLE,
	AT_COUNT,
};

struct CollisionData
{
	Vec3 Inter = { 0.f, 0.f, 0.f };
};

class AActor : public enable_shared_from_this<AActor>
{
public:
	AActor();
	virtual ~AActor() = default;

public:
	virtual void Init();
	virtual void Tick();
	virtual void Render();
	virtual void RenderShadow();
	virtual void Destroy();

	//--------------------------------------------------------------------------------------
	// Actor 상태
	//--------------------------------------------------------------------------------------
public:
	wstring m_szName;
	UINT m_Index = 0;			// 고유 번호. 해당 Actor를 빠르게 찾는 로직에서 필요하다.
	bool m_bCollision = true;	// 해당 Actor가 이번 Frame에 충돌을 하는지 확인하는 용도.
	bool m_bRender = true;		// 해당 Actor가 이번 Render에 출력되는지 확인하는 용도.
	bool m_bDelete = false;	
	bool m_bUpdateQuadTree = false;
	bool m_bUseStencil = false;
	ActorType m_eActorType = ActorType::AT_NONE;

	// Collision
	map<UINT, CollisionData> m_vCollisionList;

	void SetUseStencil(bool _isStencil) { m_bUseStencil = _isStencil; }

	//--------------------------------------------------------------------------------------
	// Component
	//--------------------------------------------------------------------------------------
protected:
	friend class UCameraComponent;
	unique_ptr<USceneComponent> m_pTransform = nullptr;
	unique_ptr<UPhysicsComponent> m_pPhysics = nullptr;
	array<shared_ptr<USceneComponent>, static_cast<size_t>(ComponentType::CT_COUNT)> m_arrComponent;
	vector<shared_ptr<class UScriptComponent>> m_vScript;
	std::string m_sPrefabPath = {  };

public:
	USceneComponent* GetTransform() { return m_pTransform.get(); }
	UPhysicsComponent* GetPhysicsComponent() { return m_pPhysics.get(); }
	template<typename T>
	shared_ptr<T> GetMeshComponent() { return static_pointer_cast<T>(m_arrComponent[static_cast<size_t>(ComponentType::CT_MESH)]); }
	shared_ptr<UMeshComponent> GetMeshComponent() { return static_pointer_cast<UMeshComponent>(m_arrComponent[static_cast<size_t>(ComponentType::CT_MESH)]); }
	shared_ptr<UCameraComponent> GetCameraComponent() { return static_pointer_cast<UCameraComponent>(m_arrComponent[static_cast<size_t>(ComponentType::CT_CAMERA)]); }
	shared_ptr<UShapeComponent> GetShapeComponent() { return static_pointer_cast<UShapeComponent>(m_arrComponent[static_cast<size_t>(ComponentType::CT_SHAPE)]); }
	shared_ptr<ULightComponent> GetLightComponent() { return static_pointer_cast<ULightComponent>(m_arrComponent[static_cast<size_t>(ComponentType::CT_LIGHT)]); }

	void SetMeshComponent(shared_ptr<UMeshComponent> _mesh) { m_arrComponent[static_cast<size_t>(ComponentType::CT_MESH)] = static_pointer_cast<USceneComponent>(_mesh); }
	void SetCameraComponent(shared_ptr<UCameraComponent> _camera) { m_arrComponent[static_cast<size_t>(ComponentType::CT_CAMERA)] = static_pointer_cast<USceneComponent>(_camera); }
	void SetShapeComponent(shared_ptr<UShapeComponent> _shape) { m_arrComponent[static_cast<size_t>(ComponentType::CT_SHAPE)] = static_pointer_cast<USceneComponent>(_shape); }
	void SetLightComponent(shared_ptr<ULightComponent> _light) { m_arrComponent[static_cast<size_t>(ComponentType::CT_LIGHT)] = static_pointer_cast<USceneComponent>(_light); }
		
	// Script
	void AddScript(shared_ptr<class UScriptComponent> _script) { m_vScript.push_back(_script); }
	const std::vector<std::shared_ptr<class UScriptComponent>>& GetScriptList() const { return m_vScript; }

	// PrefabData
	const std::string& GetPrefabPath() const { return m_sPrefabPath; }
	void SetPrefabPath(const std::string& path) { m_sPrefabPath = path; }

public:
	const Vec3& GetPosition() const { return m_pTransform->GetLocalPosition(); }
	const Vec3& GetRotation() const { return m_pTransform->GetLocalRotation(); }
	const Vec3& GetScale() const { return m_pTransform->GetLocalScale(); }
	const Vec3& GetLook() const { return m_pTransform->GetLocalLook(); }
	const Vec3& GetRight() const { return m_pTransform->GetLocalRight(); }
	const Vec3& GetUp() const { return m_pTransform->GetLocalUp(); }
	const Matrix& GetWorld() const { return m_pTransform->GetWorld(); }

	void SetPosition(const Vec3& _pos) { m_pTransform->SetLocalPosition(_pos); }
	void SetRotation(const Vec3& _rot) { m_pTransform->SetLocalRotation(_rot); }
	void SetScale(const Vec3& _scale) { m_pTransform->SetLocalScale(_scale); }
	void SetWorldMatrix(const Matrix& _mat) { m_pTransform->SetWorldMatrix(_mat); }
	void AddPosition(const Vec3& _pos) { m_pTransform->AddLocalPosition(_pos); }
	void AddRotation(const Vec3& _rot) { m_pTransform->AddLocalRotation(_rot); }

public:	
	void SetMove(const Vec3& _vDir, const float& _fMaxSpeed, const float& _fAccle = 1.0f) { m_pPhysics->SetMove(_vDir, _fMaxSpeed, _fAccle); }

	//shadow
public:
	void SetCastShadow(bool b) { m_bCastShadow = b; }
	bool IsCastShadow() const { return m_bCastShadow; }

public:
	bool m_bCastShadow = true;

};