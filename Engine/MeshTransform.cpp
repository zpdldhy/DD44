#include "pch.h"
#include "MeshTransform.h"

void MeshTransform::Init()
{
}

void MeshTransform::Tick()
{
}

void MeshTransform::SetLocalScale(const Vec3& _scale)
{
	m_vLocalScale = _scale;
	UpdateMatrix();
}

void MeshTransform::SetLocalRotation(const Vec3& _rotation)
{
	m_vLocalRotation = _rotation;
	UpdateMatrix();
}

void MeshTransform::AddLocalRotation(const Vec3& _rotation)
{
	m_vLocalRotation += _rotation;
	UpdateMatrix();
}

void MeshTransform::SetLocalPosition(const Vec3& _position)
{
	m_vLocalPosition = _position;
	UpdateMatrix();
}

void MeshTransform::SetWorldScale(const Vec3& _scale)
{
	if (!m_pParent.expired())
	{
		Vec3 parentScale = m_pParent.lock()->GetWorldScale();
		Vec3 scale = _scale;
		scale.x /= parentScale.x;
		scale.y /= parentScale.y;
		scale.z /= parentScale.z;
		SetLocalScale(scale);
	}
	else
	{
		SetLocalScale(_scale);
	}
}

void MeshTransform::SetWorldRotation(const Vec3& _rotation)
{
	if (HasParent())
	{
		Matrix inverseMatrix = m_pParent.lock()->GetWorldMat().Invert();

		Vec3 rotation;
		rotation.TransformNormal(_rotation, inverseMatrix);

		SetLocalRotation(rotation);
	}
	else
		SetLocalRotation(_rotation);
}

void MeshTransform::SetWorldPosition(const Vec3& _position)
{
	if (!m_pParent.expired())
	{
		Matrix worldToParentLocalMatrix = m_pParent.lock()->GetWorldMat().Invert();

		Vec3 position;
		position.Transform(_position, worldToParentLocalMatrix);

		SetLocalPosition(position);
	}
	else
	{
		SetLocalPosition(_position);
	}
}

void MeshTransform::SetAnimMat(const Matrix& _anim)
{
	m_matAnim = _anim;
}

void MeshTransform::UpdateMatrix()
{
	UpdateRotationMatrix();
	m_matScale = Matrix::CreateScale(m_vLocalScale);
	m_matPosition = Matrix::CreateTranslation(m_vLocalPosition);

	m_matLocal = m_matScale * m_matRotation * m_matPosition;

	m_vLocalRight = { m_matWorld._11, m_matWorld._12, m_matWorld._13 };
	m_vLocalRight.Normalize();
	m_vLocalUp = { m_matWorld._21, m_matWorld._22, m_matWorld._23 };
	m_vLocalUp.Normalize();
	m_vLocalLook = { m_matWorld._31, m_matWorld._32, m_matWorld._33 };
	m_vLocalLook.Normalize();

	if (HasParent())
	{
		m_matParent = m_pParent.lock()->GetWorldMat();
	}

	m_matWorld = m_matLocal * m_matAnim * m_matParent;
}

void MeshTransform::UpdateRotationMatrix()
{

	m_matRotation = Matrix::CreateRotationZ(m_vLocalRotation.z);
	m_matRotation *= Matrix::CreateRotationX(m_vLocalRotation.x);
	m_matRotation *= Matrix::CreateRotationY(m_vLocalRotation.y);

	//float yaw = DirectX::XMConvertToRadians(m_vLocalRotation.y);
	//float pitch = DirectX::XMConvertToRadians(m_vLocalRotation.x);
	//float roll = DirectX::XMConvertToRadians(m_vLocalRotation.z);

	//Quaternion quat;
	//quat = quat.CreateFromYawPitchRoll(yaw, pitch, roll);

	//Matrix mat;
	//mat = mat.CreateFromQuaternion(quat);

	//m_matRotation = mat;
}

shared_ptr<MeshTransform> MeshTransform::Clone()
{
	shared_ptr<MeshTransform> transform = make_shared<MeshTransform>();
	transform->m_vLocalScale = m_vLocalScale;
	transform->m_vLocalRotation = m_vLocalRotation;
	transform->m_vLocalPosition = m_vLocalPosition;
	transform->m_vLocalLook = m_vLocalLook;
	transform->m_vLocalRight = m_vLocalRight;
	transform->m_vLocalUp = m_vLocalUp;

	transform->m_vWorldScale = m_vWorldScale;
	transform->m_vWorldRotation = m_vWorldRotation;
	transform->m_vWorldPosition = m_vWorldPosition;
	transform->m_vWorldLook = m_vWorldLook;
	transform->m_vWorldRight = m_vWorldRight;
	transform->m_vWorldUp = m_vWorldUp;

	transform->m_matLocal = m_matLocal;
	transform->m_matWorld = m_matWorld;
	transform->m_matScale = m_matScale;
	transform->m_matRotation = m_matRotation;
	transform->m_matPosition = m_matPosition;
	transform->m_matParent = m_matParent;
	transform->m_matAnim = m_matAnim;

	weak_ptr<MeshTransform> m_pParent;
	vector<weak_ptr<MeshTransform>> m_vChild;

	return shared_ptr<MeshTransform>();
}
