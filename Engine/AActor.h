#pragma once
#include "UObject.h"
#include "UMeshComponent.h"
#include "UCameraComponent.h"

struct cbData
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};

enum ComponentType
{
	CT_MESH,
	CT_COUNT,
};

class AActor : public UObject
{
protected:
	array<shared_ptr<UPrimitiveComponent>, ComponentType::CT_COUNT> m_Components;
	friend class UCameraComponent;

	cbData m_cbData;
	ComPtr<ID3D11Buffer> m_pWorldCB;

protected:
	Vec3 m_vLook	= { 0.0f,0.0f,1.0f };
	Vec3 m_vRight	= { 1.0f,0.0f,0.0f };
	Vec3 m_vUp		= { 0.0f,1.0f,0.0f };

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
	template<typename T>
	shared_ptr<T> GetMesh() { return static_pointer_cast<T>(m_Components[ComponentType::CT_MESH]); }

	void SetMesh(shared_ptr<UMeshComponent> _mesh) { m_Components[ComponentType::CT_MESH] = _mesh; }

public:
	const Vec3& GetPosition() const { return m_vPosition; }
	const Vec3& GetRotation() const { return m_vRotation; }
	const Vec3& GetScale() const { return m_vScale; }

	void SetPosition(const Vec3& _pos) { m_vPosition = _pos; }
	void SetRotation(const Vec3& _rot) { m_vRotation = _rot; }
	void SetScale(const Vec3& _scale) { m_vScale = _scale; }
	void AddPosition(const Vec3& _pos) { m_vPosition += _pos; }
	void AddRotation(const Vec3& _rot) { m_vRotation += _rot; }	
};

// AActor
// 위치, 회전, 스케일
// Component 정보
