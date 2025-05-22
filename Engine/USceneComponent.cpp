#include "pch.h"
#include "USceneComponent.h"
#include "Device.h"
#include "AActor.h"

ComPtr<ID3D11Buffer> USceneComponent::m_pWorldCB = nullptr;

void USceneComponent::Init()
{
	CreateConstantBuffer();

	// Transform을 제외한 Component의 Parent 지정
	if (m_pOwner.lock()->GetTransform().get() != this)
	{
		m_pParentTransform = m_pOwner.lock()->GetTransform();
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

	m_vLocalLook.x = m_matLocal._31; m_vLocalLook.y = m_matLocal._32; m_vLocalLook.z = m_matLocal._33;
	m_vLocalRight.x = m_matLocal._11; m_vLocalRight.y = m_matLocal._12; m_vLocalRight.z = m_matLocal._13;
	m_vLocalUp.x = m_matLocal._21; m_vLocalUp.y = m_matLocal._22; m_vLocalUp.z = m_matLocal._23;

	m_vLocalLook.Normalize();
	m_vLocalRight.Normalize();
	m_vLocalUp.Normalize();

	// Set World
	// No Parent(Transform)
	if (m_pParentTransform == nullptr)
	{
		m_vWorldLook = m_vLocalLook;
		m_vWorldRight = m_vLocalRight;
		m_vWorldUp = m_vLocalUp;

		m_vWorldScale = m_vLocalScale;
		m_vWorldRotation = m_vLocalRotation;
		m_vWorldPosition = m_vLocalPosition;
	}
	// Actor - Actor && Actor - Component
	else
	{
		m_vWorldLook = m_pParentTransform->GetLocalLook();
		m_vWorldRight = m_pParentTransform->GetLocalRight();
		m_vWorldUp = m_pParentTransform->GetLocalUp();

		m_vWorldScale = m_vLocalScale * m_pParentTransform->GetLocalScale();
		m_vWorldRotation = m_vLocalRotation + m_pParentTransform->GetLocalRotation();
		m_vWorldPosition = m_vLocalPosition + m_pParentTransform->GetLocalPosition();

		m_matParent = m_pParentTransform->GetWorld();
	}

	// Set Matrix
	m_matLocal = m_matScale * m_matRotation * m_matTrans;
	m_matWorld = m_matLocal * m_matAnim * m_matParent;
}

const Vec3& USceneComponent::GetAnimWorld()
{
	auto mat = m_matLocal * m_matAnim;

	m_vAnimWorld.x = mat._41;
	m_vAnimWorld.y = mat._42;
	m_vAnimWorld.z = mat._43;

	return m_vAnimWorld;
}
