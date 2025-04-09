#pragma once
#include "UObject.h"
#include "UMeshComponent.h"

struct cbData
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;
};

class AActor : public UObject
{
protected:
	shared_ptr<UMeshComponent> m_pMesh;

	cbData m_cbData;
	ComPtr<ID3D11Buffer> m_pConstantBuffer;

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
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;
public:
	virtual bool CreateConstantBuffer();

	void UpdateWorldMatrix();
public:
	UMeshComponent* GetMesh() { return m_pMesh.get(); }
	const Vec3& GetPosition() const { return m_vPosition; }
	const Vec3& GetRotation() const { return m_vRotation; }
	const Vec3& GetScale() const { return m_vScale; }

	void SetMesh(shared_ptr<UMeshComponent> _mesh) { m_pMesh = _mesh; }
	void SetPosition(const Vec3& _pos) { m_vPosition = _pos; }
	void SetRotation(const Vec3& _rot) { m_vRotation = _rot; }
	void SetScale(const Vec3& _scale) { m_vScale = _scale; }
};

// AActor
// 위치, 회전, 스케일
// Component 정보
