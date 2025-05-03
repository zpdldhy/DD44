#pragma once
#include "UActorComponent.h"

struct cbData
{
	Matrix matWorld;
};

class USceneComponent : public UActorComponent
{
protected:
	cbData m_cbData;
	ComPtr<ID3D11Buffer> m_pWorldCB;

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
	USceneComponent() = default;
	virtual ~USceneComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

private:
	virtual bool CreateConstantBuffer();
	void UpdateWorldMatrix();

public:
	const Vec3 GetPosition() const { return m_vPosition; }
	const Vec3 GetRotation() const { return m_vRotation; }
	const Vec3 GetScale() const { return m_vScale; }
	const Vec3& GetLook() const { return m_vLook; }
	const Vec3& GetRight() const { return m_vRight; }
	const Vec3& GetUp() const { return m_vUp; }

	void SetPosition(const Vec3& _pos) { m_vPosition = _pos; }
	void SetRotation(const Vec3& _rot) { m_vRotation = _rot; }
	void SetScale(const Vec3& _scale) { m_vScale = _scale; }

	void AddPosition(const Vec3& _pos) { m_vPosition += _pos; }
	void AddRotation(const Vec3& _rot) { m_vRotation += _rot; }
	void AddScale(const Vec3& _scale) { m_vScale += _scale; }
};

