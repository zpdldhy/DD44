#include "pch.h"
#include "CameraManager.h"
#include "Device.h"
#include "ACameraActor.h"
#include "UCameraComponent.h"
#include "Timer.h"
#include "ObjectManager.h"
#include "QuadTree.h"
#include "CollisionManager.h"

void CameraManager::Init()
{	
	// No CameraActor!! 
	// Set in client
	if(m_p3DCameraActor == nullptr)
		assert(false);

	m_pUICameraActor = make_shared<ACameraActor>(ProjectionType::PT_ORTHOGRAPHIC);
	{
		m_pUICameraActor->SetPosition(Vec3(0.f, 0.f, -1.f));
		m_pUICameraActor->Init();
		m_pUIComponent = m_pUICameraActor->GetCameraComponent();
	}

	CreateCameraBuffer();
}

void CameraManager::Tick()
{
	if (m_p3DCameraActor)
	{
		m_vPrevCameraPos = m_vCurrentCameraPos;
		m_vCurrentCameraPos = m_p3DCameraActor->GetPosition();
	}

	m_pUICameraActor->Tick();
	m_p3DCameraActor->GetCameraComponent()->SetFrustumVisible(false);

	//UpdateFrustumPlanes();

	//auto QuadRoot = OBJECT->GetQuadTree()->GetRoot();
	//FrustumCulling(QuadRoot);

	//for (auto& pNode : m_vCulledNodes)
	//	for (auto& iIndex : pNode->vActorIndices)
	//	{
	//		auto pActor = OBJECT->GetActor(iIndex);
	//		if(pActor!=nullptr)
	//			pActor->m_bRender = true;
	//	}

	//m_vCulledNodes.clear();
}

void CameraManager::Render(CameraViewType _ViewType)
{
	shared_ptr<UCameraComponent> pCurrentComponent = nullptr;

	if (_ViewType == CameraViewType::CVT_ACTOR)
		pCurrentComponent = m_p3DCameraActor->GetCameraComponent();
	else if (_ViewType == CameraViewType::CVT_UI)
		pCurrentComponent = m_pUICameraActor->GetCameraComponent();

	m_CameraData.matView = pCurrentComponent->GetView();
	m_CameraData.matProjection = pCurrentComponent->GetProjection();
	m_CameraData.g_vCameraPos = pCurrentComponent->GetWorldPosition();
	m_CameraData.GameTime = TIMER->GetGameTime();

	DC->UpdateSubresource(m_pCameraCB.Get(), 0, nullptr, &m_CameraData, 0, 0);
	DC->VSSetConstantBuffers(1, 1, m_pCameraCB.GetAddressOf());
	DC->PSSetConstantBuffers(1, 1, m_pCameraCB.GetAddressOf());
}

void CameraManager::Destroy()
{

}

void CameraManager::CreateCameraBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(CameraConstantData);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, nullptr, m_pCameraCB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateCameraBuffer Failed"));
		assert(false);
	}
}

void CameraManager::UpdateFrustumPlanes()
{
	if(m_p3DCameraActor == nullptr || m_p3DComponent == nullptr)
		return;

	auto ComboMatrix = m_p3DComponent->GetViewProjection();

	// Left clipping plane 
	m_FrustumPlanes[0].x = ComboMatrix._14 + ComboMatrix._11;
	m_FrustumPlanes[0].y = ComboMatrix._24 + ComboMatrix._21;
	m_FrustumPlanes[0].z = ComboMatrix._34 + ComboMatrix._31;
	m_FrustumPlanes[0].w = ComboMatrix._44 + ComboMatrix._41;
	// Right clipping plane 
	m_FrustumPlanes[1].x = ComboMatrix._14 - ComboMatrix._11;
	m_FrustumPlanes[1].y = ComboMatrix._24 - ComboMatrix._21;
	m_FrustumPlanes[1].z = ComboMatrix._34 - ComboMatrix._31;
	m_FrustumPlanes[1].w = ComboMatrix._44 - ComboMatrix._41;
	// Top clipping plane 
	m_FrustumPlanes[2].x = ComboMatrix._14 - ComboMatrix._12;
	m_FrustumPlanes[2].y = ComboMatrix._24 - ComboMatrix._22;
	m_FrustumPlanes[2].z = ComboMatrix._34 - ComboMatrix._32;
	m_FrustumPlanes[2].w = ComboMatrix._44 - ComboMatrix._42;
	// Bottom clipping plane 
	m_FrustumPlanes[3].x = ComboMatrix._14 + ComboMatrix._12;
	m_FrustumPlanes[3].y = ComboMatrix._24 + ComboMatrix._22;
	m_FrustumPlanes[3].z = ComboMatrix._34 + ComboMatrix._32;
	m_FrustumPlanes[3].w = ComboMatrix._44 + ComboMatrix._42;
	// Near clipping plane 
	m_FrustumPlanes[4].x = ComboMatrix._13;
	m_FrustumPlanes[4].y = ComboMatrix._23;
	m_FrustumPlanes[4].z = ComboMatrix._33;
	m_FrustumPlanes[4].w = ComboMatrix._43;
	// xr clipping plane 
	m_FrustumPlanes[5].x = ComboMatrix._14 - ComboMatrix._13;
	m_FrustumPlanes[5].y = ComboMatrix._24 - ComboMatrix._23;
	m_FrustumPlanes[5].z = ComboMatrix._34 - ComboMatrix._33;
	m_FrustumPlanes[5].w = ComboMatrix._44 - ComboMatrix._43;

	m_FrustumPlanes[0].Normalize();
	m_FrustumPlanes[1].Normalize();
	m_FrustumPlanes[2].Normalize();
	m_FrustumPlanes[3].Normalize();
	m_FrustumPlanes[4].Normalize();
	m_FrustumPlanes[5].Normalize();
}

void CameraManager::FrustumCulling(QuadTreeNode* pNode)
{
	if(pNode->pChildren[0] == nullptr)
		return;

	vector<QuadTreeNode*> vCulledNodes;

	// 자식 Node 검사
	for (int iChildIndex = 0; iChildIndex < 4; iChildIndex++)
	{
		// Frustum 컬링
		if (Collision::CheckAABBToPlane(pNode->pChildren[iChildIndex]->bounds, m_FrustumPlanes[0]) < 0)continue;
		if (Collision::CheckAABBToPlane(pNode->pChildren[iChildIndex]->bounds, m_FrustumPlanes[1]) < 0)continue;
		if (Collision::CheckAABBToPlane(pNode->pChildren[iChildIndex]->bounds, m_FrustumPlanes[2]) < 0)continue;
		if (Collision::CheckAABBToPlane(pNode->pChildren[iChildIndex]->bounds, m_FrustumPlanes[3]) < 0)continue;
		if (Collision::CheckAABBToPlane(pNode->pChildren[iChildIndex]->bounds, m_FrustumPlanes[4]) < 0)continue;
		if (Collision::CheckAABBToPlane(pNode->pChildren[iChildIndex]->bounds, m_FrustumPlanes[5]) < 0)continue;				

		vCulledNodes.emplace_back(pNode->pChildren[iChildIndex].get());
	}

	// 사용시 주석 해제
	//for (auto pChildNode : vCulledNodes) 
	//{
	//	// 자식 있으면 재귀
	//	if (pChildNode->pChildren[0] != nullptr)
	//			FrustumCulling(pChildNode);
	//	// Leaf는 가진다.
	//	else
	//		m_vCulledNodes.emplace_back(pChildNode);
	//}
}

bool CameraManager::CheckPointToPlane(const Vec3& _point) const
{
	for (int iPlane = 0; iPlane < 6; iPlane++)
	{
		auto normal = m_FrustumPlanes[iPlane].Normal();
		normal.Normalize();
		auto d = normal.Dot(_point) + m_FrustumPlanes[iPlane].w;

		if (d < 0.f) // Out of frustum
		{
			return false;
		}
	}
	return true;
}

void CameraManager::Set3DCameraActor(shared_ptr<class AActor> _cameraActor)
{
	if (_cameraActor->GetCameraComponent() == nullptr)
		return;

	m_p3DCameraActor = _cameraActor;
	m_p3DComponent = m_p3DCameraActor->GetCameraComponent();
}

shared_ptr<class AActor> CameraManager::Get3DCameraActor()
{
	return m_p3DCameraActor;
}

shared_ptr<class UCameraComponent> CameraManager::Get3DCameraComponent()
{
	return m_p3DComponent;
}

Matrix CameraManager::Get3DView()
{
	return m_p3DComponent->GetView();
}

Matrix CameraManager::Get3DProjection()
{
	return m_p3DComponent->GetProjection();
}

shared_ptr<class UCameraComponent> CameraManager::GetUICameraComponent()
{
	return m_pUIComponent;
}

Matrix CameraManager::GetUIView()
{
	return m_pUIComponent->GetView();
}

Matrix CameraManager::GetUIProjection()
{
	return m_pUIComponent->GetProjection();
}

Vec3 CameraManager::GetNDCPos(Vec3 _vWorldPos)
{
	Vec3 ret;

	Vec4 temp = Vec4(_vWorldPos.x, _vWorldPos.y, _vWorldPos.z, 1.f);
	temp = XMVector4Transform(temp, Get3DView());
	temp = XMVector4Transform(temp, Get3DProjection());

	temp /= temp.w;

	ret = Vec3(temp.x, temp.y, temp.z);

	return ret;
}

Vec3 CameraManager::GetScreenPos(Vec3 _vWorldPos)
{
	Vec3 ret = GetNDCPos(_vWorldPos);

	ret.x *= (static_cast<float>(g_windowSize.x) / 2.f);
	ret.y *= (static_cast<float>(g_windowSize.y) / 2.f);

	return ret;
}
