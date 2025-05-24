#include "pch.h"
#include "USceneComponent.h"
#include "Device.h"
#include "AActor.h"

ComPtr<ID3D11Buffer> USceneComponent::m_pWorldCB = nullptr;

void USceneComponent::Init()
{
	CreateConstantBuffer();

	// Transform을 제외한 Component의 Parent 지정
	if (GetOwner()->GetTransform().get() != this)
	{
		m_pParentTransform = GetOwner()->GetTransform();
	}
}

void USceneComponent::Tick()
{
	UpdateWorldMatrix();
}

void USceneComponent::Render()
{
	if (m_pWorldCB)
	{
		// m_cbData는 static으로 돌려 사용하기 때문에 Render 직전에 바꾼다.
		m_cbData.matWorld = m_matWorld;
		DC->UpdateSubresource(m_pWorldCB.Get(), 0, NULL, &m_cbData, 0, 0);
		DC->VSSetConstantBuffers(0, 1, m_pWorldCB.GetAddressOf());
	}
}

void USceneComponent::Destroy()
{
}


bool USceneComponent::CreateConstantBuffer()
{
	if (m_pWorldCB != nullptr)
		return true;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(cbData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_cbData;

	HRESULT hr =
		DEVICE->CreateBuffer(&bd, &sd, m_pWorldCB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateWorldBuffer Failed"));
		return false;
	}

	return true;
}

void USceneComponent::UpdateWorldMatrix()
{
	// Set Local
	m_matScale = Matrix::CreateScale(m_vLocalScale);

	m_matRotation = Matrix::CreateRotationZ(m_vLocalRotation.z);
	m_matRotation *= Matrix::CreateRotationX(m_vLocalRotation.x);
	m_matRotation *= Matrix::CreateRotationY(m_vLocalRotation.y);

	m_matTrans = Matrix::CreateTranslation(m_vLocalPosition);

	m_matLocal = m_matScale * m_matRotation * m_matTrans;

	m_vLocalLook	= Vec3(m_matLocal._31, m_matLocal._32, m_matLocal._33);
	m_vLocalRight	= Vec3(m_matLocal._11, m_matLocal._12, m_matLocal._13);
	m_vLocalUp		= Vec3(m_matLocal._21, m_matLocal._22, m_matLocal._23);

	m_vLocalLook.Normalize();
	m_vLocalRight.Normalize();
	m_vLocalUp.Normalize();

	// Set World
	// No Parent(Transform)
	if (m_pParentTransform == nullptr)
	{
		m_matWorld = m_matLocal;
	}
	// Actor - Actor && Actor - Component
	else
	{
		m_matParent = m_pParentTransform->GetWorld();
		m_matWorld = m_matLocal * m_matParent;
	}

	Quaternion qRot;
	bool success = m_matWorld.Decompose(m_vWorldScale, qRot, m_vWorldPosition);
	if (success)
	{
		// Quaternion → Euler로 변환 (선택)
		m_vWorldRotation = qRot.ToEuler(); // 이 함수는 직접 구현하거나 XMQuaternionToEulerAngles 등 사용
	}

	m_vWorldLook = Vec3(m_matWorld._11, m_matWorld._12, m_matWorld._13);
	m_vWorldRight = Vec3(m_matWorld._21, m_matWorld._22, m_matWorld._23);
	m_vWorldUp = Vec3(m_matWorld._31, m_matWorld._32, m_matWorld._33);

	m_vWorldLook.Normalize();
	m_vWorldRight.Normalize();
	m_vWorldUp.Normalize();
}
