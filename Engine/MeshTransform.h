#pragma once
class MeshTransform : public enable_shared_from_this<MeshTransform>
{
private:
	Vec3 m_vLocalScale = { 1.0f, 1.0f, 1.0f };
	Vec3 m_vLocalRotation = { 0.0f, 0.0f, 0.0f };
	Vec3 m_vLocalPosition = { 0.0f, 0.0f, 0.0f };
	Vec3 m_vLocalLook;
	Vec3 m_vLocalRight;
	Vec3 m_vLocalUp;

	Vec3 m_vWorldScale = { 1.0f, 1.0f, 1.0f };
	Vec3 m_vWorldRotation = { 0.0f, 0.0f, 0.0f };
	Vec3 m_vWorldPosition = { 0.0f, 0.0f, 0.0f };
	Vec3 m_vWorldLook;
	Vec3 m_vWorldRight;
	Vec3 m_vWorldUp;

	Matrix m_matLocal;
	Matrix m_matWorld;
	Matrix m_matScale;
	Matrix m_matRotation;
	Matrix m_matPosition;
	Matrix m_matParent;
	Matrix m_matAnim;

	weak_ptr<MeshTransform> m_pParent;
	vector<weak_ptr<MeshTransform>> m_vChild;
public:
	MeshTransform() = default;
	~MeshTransform() = default;
public:
	void Init();
	void Tick();
public:
	// Local
	void SetLocalScale(const Vec3& _scale);
	void SetLocalRotation(const Vec3& _rotation);
	void AddLocalRotation(const Vec3& _rotation);
	void SetLocalPosition(const Vec3& _position);
	const Vec3& GetLocalScale() { return m_vLocalScale; }
	const Vec3& GetLocalRotation() { return m_vLocalRotation; }
	const Vec3& GetLocalPosition() { return m_vLocalPosition; }

	// World
	void SetWorldScale(const Vec3& _scale);
	void SetWorldRotation(const Vec3& _rotation);
	void SetWorldPosition(const Vec3& _position);
	void SetAnimMat(const Matrix& _anim);
	const Vec3& GetWorldScale() { return m_vWorldScale; }
	const Vec3& GetWorldRotation() { return m_vWorldRotation; }
	const Vec3& GetWorldPosition() { return m_vWorldPosition; }
	const Matrix& GetWorldMat() { return m_matWorld; }

	// Updata
	void UpdateMatrix();
	void UpdateRotationMatrix();

public:
	void SetParent(const shared_ptr<MeshTransform>& _parent);
	void AddChild(const shared_ptr<MeshTransform>& _child) { m_vChild.push_back(_child); }
	void RemoveChild(const shared_ptr<MeshTransform>& _child);
	bool HasParent() { return m_pParent.expired() ? false : true; }

	shared_ptr<MeshTransform> Clone();
};

