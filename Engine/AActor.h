#pragma once
#include "UObject.h"
#include "UMeshComponent.h"
#include "UCameraComponent.h"

struct cbData
{
	Matrix matWorld;
};

enum class ComponentType
{
	CT_CAMERA,
	CT_MESH,
	CT_COUNT,
};

class AActor : public UObject
{
	friend class UCameraComponent;

protected:
	array<shared_ptr<USceneComponent>, static_cast<size_t>(ComponentType::CT_COUNT)> m_arrComponent;
	vector<shared_ptr<class UScriptComponent>> m_vScript;

	cbData m_cbData;
	ComPtr<ID3D11Buffer> m_pWorldCB;

protected:
	Vec3 m_vLook = { 0.0f,0.0f,1.0f };
	Vec3 m_vRight = { 1.0f,0.0f,0.0f };
	Vec3 m_vUp = { 0.0f,1.0f,0.0f };

	Vec3 m_vScale = { 1.0f, 1.0f, 1.0f };
	Vec3 m_vRotation = { 0.0f, 0.0f, 0.0f };
	Vec3 m_vPosition = { 0.0f, 0.0f, 0.0f };

	Matrix m_matOffset = Matrix::Identity;
	Matrix m_matWorld = Matrix::Identity;
	Matrix m_matScale = Matrix::Identity;
	Matrix m_matRotation = Matrix::Identity;
	Matrix m_matTrans = Matrix::Identity;
	Matrix m_matParent = Matrix::Identity;

public:
	AActor() = default;
	virtual ~AActor() = default;
public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;
public:
	virtual bool CreateConstantBuffer();

	void UpdateWorldMatrix();

public:
	// Component
	template<typename T>
	shared_ptr<T> GetMeshComponent() { return static_pointer_cast<T>(m_arrComponent[static_cast<size_t>(ComponentType::CT_MESH)]); }
	shared_ptr<UCameraComponent> GetCameraComponent() { return static_pointer_cast<UCameraComponent>(m_arrComponent[static_cast<size_t>(ComponentType::CT_CAMERA)]); }

	void SetMeshComponent(shared_ptr<UMeshComponent> _mesh) { m_arrComponent[static_cast<size_t>(ComponentType::CT_MESH)] = static_pointer_cast<USceneComponent>(_mesh); }
	void SetCameraComponent(shared_ptr<UCameraComponent> _camera) { m_arrComponent[static_cast<size_t>(ComponentType::CT_CAMERA)] = static_pointer_cast<USceneComponent>(_camera); }

	// Script
	void AddScript(shared_ptr<class UScriptComponent> _script) { m_vScript.push_back(_script); }

public:
	const Vec3& GetPosition() const { return m_vPosition; }
	const Vec3& GetRotation() const { return m_vRotation; }
	const Vec3& GetScale() const { return m_vScale; }
	const Vec3& GetLook() const { return m_vLook; }
	const Vec3& GetRight() const { return m_vRight; }
	const Vec3& GetUp() const { return m_vUp; }

	void SetPosition(const Vec3& _pos) { m_vPosition = _pos; }
	void SetRotation(const Vec3& _rot) { m_vRotation = _rot; }
	void SetScale(const Vec3& _scale) { m_vScale = _scale; }
	void AddPosition(const Vec3& _pos) { m_vPosition += _pos; }
	void AddRotation(const Vec3& _rot) { m_vRotation += _rot; }
};

// AActor
// 위치, 회전, 스케일
// Component 정보
