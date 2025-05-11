#pragma once
#include "UActorComponent.h"

struct cbData
{
	Matrix matWorld;
};

class USceneComponent : public UActorComponent
{
public:
	bool m_bUseBoneMatrix = false;

protected:
	cbData m_cbData;
	static ComPtr<ID3D11Buffer> m_pWorldCB;

	shared_ptr<USceneComponent> m_pParentTransform = nullptr;

	// Local Transform
	Vec3 m_vLocalLook = { 0.0f,0.0f,1.0f };
	Vec3 m_vLocalRight = { 1.0f,0.0f,0.0f };
	Vec3 m_vLocalUp = { 0.0f,1.0f,0.0f };

	Vec3 m_vLocalScale = { 1.0f, 1.0f, 1.0f };
	Vec3 m_vLocalRotation = { 0.0f, 0.0f, 0.0f };
	Vec3 m_vLocalPosition = { 0.0f, 0.0f, 0.0f };

	// World Transform
	Vec3 m_vWorldLook = { 0.0f,0.0f,1.0f };
	Vec3 m_vWorldRight = { 1.0f,0.0f,0.0f };
	Vec3 m_vWorldUp = { 0.0f,1.0f,0.0f };

	Vec3 m_vWorldScale = { 1.0f, 1.0f, 1.0f };
	Vec3 m_vWorldRotation = { 0.0f, 0.0f, 0.0f };
	Vec3 m_vWorldPosition = { 0.0f, 0.0f, 0.0f };

	// Matrix
	Matrix m_matLocal = Matrix::Identity;
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
	// Local
	const Vec3& GetLocalPosition() const { return m_vLocalPosition; }
	const Vec3& GetLocalRotation() const { return m_vLocalRotation; }
	const Vec3& GetLocalScale() const { return m_vLocalScale; }
	const Vec3& GetLocalLook() const { return m_vLocalLook; }
	const Vec3& GetLocalRight() const { return m_vLocalRight; }
	const Vec3& GetLocalUp() const { return m_vLocalUp; }

	void SetLocalPosition(const Vec3& _pos) { m_vLocalPosition = _pos; }
	void SetLocalRotation(const Vec3& _rot) { m_vLocalRotation = _rot; }
	void SetLocalScale(const Vec3& _scale) { m_vLocalScale = _scale; }

	void AddLocalPosition(const Vec3& _pos) { m_vLocalPosition += _pos; }
	void AddLocalRotation(const Vec3& _rot) { m_vLocalRotation += _rot; }
	void AddLocalScale(const Vec3& _scale) { m_vLocalScale += _scale; }

	// World
	const Vec3& GetWorldPosition() const { return m_vWorldPosition; }
	const Vec3& GetWorldRotation() const { return m_vWorldRotation; }
	const Vec3& GetWorldScale() const { return m_vWorldScale; }
	const Vec3& GetWorldLook() const { return m_vWorldLook; }
	const Vec3& GetWorldRight() const { return m_vWorldRight; }
	const Vec3& GetWorldUp() const { return m_vWorldUp; }

	// Matrix
	const Matrix& GetWorld() const { return m_matWorld; }
	void SetParentTransform(shared_ptr<USceneComponent> _pTransform) { if (_pTransform.get() != this) m_pParentTransform = _pTransform; }
};

