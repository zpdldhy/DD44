#pragma once
#include "UObject.h"
#include "UMeshComponent.h"
#include "UCameraComponent.h"
#include "UShapeComponent.h"
#include "ULightComponent.h"

enum class ComponentType
{
	CT_CAMERA,
	CT_SHAPE,
	//CT_EFFECT,
	CT_TRANSFORM,
	CT_MESH,
	CT_LIGHT,
	CT_COUNT,
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
	virtual void Destroy();

	//--------------------------------------------------------------------------------------
	// Actor 상태
	//--------------------------------------------------------------------------------------
protected:
	UINT m_ActorIndex = 0;	// 고유 번호. 해당 Actor를 빠르게 찾는 로직에서 필요하다.
	bool m_bDelete = false;	

public:
	void SetActorIndex(UINT _iIndex) { m_ActorIndex = _iIndex; }
	UINT GetActorIndex() { return m_ActorIndex; }

	bool IsDelete() { return m_bDelete; }
	void SetDelete(bool _bDelete) { m_bDelete = _bDelete; }	

	//--------------------------------------------------------------------------------------
	// Component
	//--------------------------------------------------------------------------------------
protected:
	friend class UCameraComponent;
	shared_ptr<USceneComponent> m_pTransform = nullptr;
	array<shared_ptr<USceneComponent>, static_cast<size_t>(ComponentType::CT_COUNT)> m_arrComponent;
	vector<shared_ptr<class UScriptComponent>> m_vScript;

public:
	template<typename T>
	shared_ptr<T> GetMeshComponent() { return static_pointer_cast<T>(m_arrComponent[static_cast<size_t>(ComponentType::CT_MESH)]); }
	shared_ptr<UCameraComponent> GetCameraComponent() { return static_pointer_cast<UCameraComponent>(m_arrComponent[static_cast<size_t>(ComponentType::CT_CAMERA)]); }
	template<typename T>
	shared_ptr<T> GetShapeComponent() { return static_pointer_cast<T>(m_arrComponent[static_cast<size_t>(ComponentType::CT_SHAPE)]); }
	template<typename T>
	shared_ptr<T> GetLightComponent() { return static_pointer_cast<T>(m_arrComponent[static_cast<size_t>(ComponentType::CT_LIGHT)]); }

	void SetMeshComponent(shared_ptr<UMeshComponent> _mesh) { m_arrComponent[static_cast<size_t>(ComponentType::CT_MESH)] = static_pointer_cast<USceneComponent>(_mesh); }
	void SetCameraComponent(shared_ptr<UCameraComponent> _camera) { m_arrComponent[static_cast<size_t>(ComponentType::CT_CAMERA)] = static_pointer_cast<USceneComponent>(_camera); }
	void SetShapeComponent(shared_ptr<UShapeComponent> _shape) { m_arrComponent[static_cast<size_t>(ComponentType::CT_SHAPE)] = static_pointer_cast<USceneComponent>(_shape); }
	void SetLightComponent(shared_ptr<ULightComponent> _light) { m_arrComponent[static_cast<size_t>(ComponentType::CT_LIGHT)] = static_pointer_cast<USceneComponent>(_light); }
		
	// Script
	void AddScript(shared_ptr<class UScriptComponent> _script) { m_vScript.push_back(_script); }

public:
	const Vec3& GetPosition() const { return m_pTransform->GetPosition(); }
	const Vec3& GetRotation() const { return m_pTransform->GetRotation(); }
	const Vec3& GetScale() const { return m_pTransform->GetScale(); }
	const Vec3& GetLook() const { return m_pTransform->GetLook(); }
	const Vec3& GetRight() const { return m_pTransform->GetRight(); }
	const Vec3& GetUp() const { return m_pTransform->GetUp(); }

	void SetPosition(const Vec3& _pos) { m_pTransform->SetPosition(_pos); }
	void SetRotation(const Vec3& _rot) { m_pTransform->SetRotation(_rot); }
	void SetScale(const Vec3& _scale) { m_pTransform->SetScale(_scale); }
	void AddPosition(const Vec3& _pos) { m_pTransform->AddPosition(_pos); }
	void AddRotation(const Vec3& _rot) { m_pTransform->AddRotation(_rot); }
};

// AActor
// 위치, 회전, 스케일
// Component 정보
