#include "pch.h"
#include "ObjectManager.h"
#include "AActor.h"
#include "AInstance.h"
#include "UMeshComponent.h"
#include "UMaterial.h"
#include "DxState.h"
#include "ATerrainTileActor.h"

// Component
#include "UBoxComponent.h"
#include "USphereComponent.h"

UINT ObjectManager::ActorCount = 0;
ComPtr<ID3D11Buffer> ObjectManager::m_pRenderModeBuffer = nullptr;

void ObjectManager::Init()
{
	CreateRenderModeCB();
	m_QuadTree.Create(8, 8, 40.0f);
}

void ObjectManager::Tick()
{
	for (auto pActor = m_vActorList.begin();pActor!=m_vActorList.end();)
	{
		// �״°ǰ�
		if (pActor->second->m_bDelete == true)
		{
			pActor = m_vActorList.erase(pActor);
		}
		else
		{
			// ���� ���͸� ����Ʈ�� ��ġ ���� - sy
			if (pActor->second->m_bUpdateQuadTree)
			{
				pActor->second->Tick();
				m_QuadTree.UpdateActor(pActor->second);
			}

 			//pActor->second->Tick();
			pActor++;
		}		
	}

	if (m_pCursor)
		m_pCursor->Tick();
}

void ObjectManager::Render()
{
	CheckStencilList();

	for (auto& pRenderActor : m_vPreRenderActorList)
	{
		// [1] Actor1 ���� ���� ������ (����, ���ٽ� ��� X)
		SetRenderMode(ERenderMode::Default);
		DC->OMSetDepthStencilState(STATE->m_pDSSDepthEnable.Get(), 0);
		pRenderActor->Render();

		// [2] Actor1 ��ġ�� ���ٽ� = 1 ���� (���� �׽�Ʈ�� �ϵ� ��� X)
		SetRenderMode(ERenderMode::Default);
		DC->OMSetDepthStencilState(STATE->m_pDSS_StencilWrite.Get(), 1);
		pRenderActor->Render();
	}

	for (auto& pRenderActor : m_vPostRenderActorList)
	{
		// [3] Actor2 �Ƿ翧���� ���ٽ� == 1 ������ ��� (���� Disable)
		SetRenderMode(ERenderMode::Silhouette);
		DC->OMSetDepthStencilState(STATE->m_pDSS_StencilMaskEqual.Get(), 1);
		pRenderActor->Render();

		// [4] Actor2 �Ϲ� ������ (���� Enable)
		SetRenderMode(ERenderMode::Default);
		DC->OMSetDepthStencilState(STATE->m_pDSSDepthEnable.Get(), 0);
		pRenderActor->Render();
	}

	if (m_pCursor)
	{
		DC->OMSetDepthStencilState(STATE->m_pDSSDepthDisableZero.Get(), 0);
		m_pCursor->Render();
	}

	ClearRenderList();
}

void ObjectManager::Destroy()
{
	m_vActorList.clear();
	m_vInstanceList.clear();
	ClearRenderList();
	ActorCount = 0;
}

void ObjectManager::AddActor(shared_ptr<class AActor> _pActor)
{
	_pActor->m_Index = ActorCount;
	m_vActorList.insert(make_pair(ActorCount, _pActor));
	m_vActorIndexList.emplace_back(ActorCount);		// �ӽ� ���
	ActorCount++;
	
	_pActor->Init();

	SetInstance(_pActor);

	// ���� ������ ��� ���͸� ����Ʈ���� ���� 1ȸ ��� - sy
	if (_pActor->m_szName != L"Terrain")
	{
		m_QuadTree.InsertActor(m_QuadTree.GetRoot(), _pActor);
	}
}

void ObjectManager::AddActorList(vector<shared_ptr<class AActor>> _vActorList)
{
	for (auto& pActor : _vActorList)
	{
		pActor->m_Index = ActorCount;
		m_vActorList.insert(make_pair(ActorCount, pActor));

		m_vActorIndexList.emplace_back(ActorCount);	// �ӽ� ���
		ActorCount++;

		pActor->Init();
		SetInstance(pActor);

		// ���� ������ ��� ���͸� ����Ʈ���� ���� 1ȸ ��� - sy
		if (pActor->m_szName != L"Terrain")
		{
			m_QuadTree.InsertActor(m_QuadTree.GetRoot(), pActor);
		}
	}
}

std::shared_ptr<class ATerrainTileActor> ObjectManager::FindTileActor()
{
	const auto& actorMap = GetActorList();

	for (const auto& pair : actorMap)
	{
		auto actor = pair.second;
		if (!actor)
			continue;

		if (actor->m_szName == L"Terrain")
		{
			return std::dynamic_pointer_cast<ATerrainTileActor>(actor);
		}
	}

	return nullptr;
}

void ObjectManager::RemoveActor(std::shared_ptr<class AActor> _pActor)
{
	_pActor->m_bDelete = true;
}

void ObjectManager::RemoveAll()
{
	ActorCount = 0;
	m_vActorList.clear();
	ClearRenderList();
}

shared_ptr<class AActor> ObjectManager::GetActor(UINT _iIndex)
{
	auto it = m_vActorList.find(_iIndex);
	if (it != m_vActorList.end())
	{
		return it->second;
	}

	return nullptr;
}

const map<UINT, shared_ptr<AActor>>& ObjectManager::GetActorList() const
{
	return m_vActorList;
}

void ObjectManager::RenderShadow()
{
	for (auto& pair : m_vActorList)
	{
		auto& actor = pair.second;
		if (!actor || !actor->IsCastShadow()) continue;

		actor->RenderShadow();
	}
}

void ObjectManager::ObjectMove()
{
	for (auto iter : m_vActorList)
	{
		auto pActor = iter.second;

		pActor->GetPhysicsComponent()->Tick();
		if (pActor->GetShapeComponent())
		{
			pActor->GetShapeComponent()->UpdateMatrix();
			pActor->GetShapeComponent()->UpdateBounds();
		}
	}
}

void ObjectManager::CollisionStabilization()
{
	for (auto iter : m_vActorList)
	{
		auto pActor = iter.second;
		if (pActor->m_vCollisionList.empty()) continue;
		auto pShape = pActor->GetShapeComponent();
		if (pShape == nullptr) continue;
		
		for (auto& ColData : pActor->m_vCollisionList)
		{
			auto pObj = GetActor(ColData.first);
			auto ObjShape = pObj->GetShapeComponent();
			if (ObjShape->GetCollisionType() == CollisionEnabled::CE_QUERYONLY) continue;

			auto inter = ColData.second.Inter;
			auto diff = pShape->GetCenter() - inter;
			auto normal = diff;
			normal.Normalize();

			float len = 0.f;

			if (pShape->GetShapeType() == ShapeType::ST_SPHERE) 
			{
				auto pSphere = dynamic_pointer_cast<USphereComponent>(pShape);				
				len = pSphere->GetBounds().fRadius - diff.Length();
			}

			pActor->AddPosition(normal * len);
			if (pActor->GetShapeComponent())
			{
				pActor->GetShapeComponent()->UpdateMatrix();
				pActor->GetShapeComponent()->UpdateBounds();
			}
		}
	}
}

void ObjectManager::SetCursorActor(shared_ptr<AActor> _pCursor)
{
	m_pCursor = _pCursor;

	if (_pCursor)
	{
		HCURSOR hCursor = LoadCursorFromFile(L"../Resources/Texture/UI/Empty.cur");
		SetCursor(hCursor);
	}
	else
	{
		SetCursor(LoadCursor(nullptr, IDC_ARROW));
	}
}

void ObjectManager::CheckStencilList()
{
	for (auto& iter : m_vActorList)
	{
		auto pActor = iter.second;
		if (pActor->m_bRender == false) continue;
		if (pActor->GetMeshComponent() == nullptr || pActor->GetMeshComponent()->IsUseInstance()) continue;

		if (pActor->m_bUseStencil == false)
			m_vPreRenderActorList.emplace_back(pActor);
		else
			m_vPostRenderActorList.emplace_back(pActor);
	}

	for (auto& pActor : m_vInstanceList)
	{
		if (pActor->m_bRender == false) continue;

		if (pActor->m_bUseStencil == false)
			m_vPreRenderActorList.emplace_back(pActor);
		else
			m_vPostRenderActorList.emplace_back(pActor);
	}
}

void ObjectManager::ClearRenderList()
{
	m_vPreRenderActorList.clear();
	m_vPostRenderActorList.clear();
	//m_vActorIndexList.clear();	// �ӽ� ���
}

void ObjectManager::CreateRenderModeCB()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(RenderModeCB);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA src;
	ZeroMemory(&src, sizeof(src));
	src.pSysMem = &m_tRenderModeData;

	HRESULT hr = DEVICE->CreateBuffer(&desc, &src, m_pRenderModeBuffer.GetAddressOf());
	assert(SUCCEEDED(hr) && "Failed to create RenderMode ConstantBuffer");
}

void ObjectManager::SetRenderMode(ERenderMode _eMode)
{
	m_tRenderModeData.iRenderMode = static_cast<int>(_eMode);

	DC->UpdateSubresource(m_pRenderModeBuffer.Get(), 0, nullptr, &m_tRenderModeData, 0, 0);

	if (m_pRenderModeBuffer)
	{
		DC->VSSetConstantBuffers(7, 1, m_pRenderModeBuffer.GetAddressOf());
		DC->PSSetConstantBuffers(7, 1, m_pRenderModeBuffer.GetAddressOf());
	}
}

void ObjectManager::SetInstance(shared_ptr<AActor> _pActor)
{
	if (_pActor->m_szName != L"Object")
		return;

	auto meshCom = _pActor->GetMeshComponent();

	for (auto& pInstance : m_vInstanceList)
	{
		// Mesh�� �ٸ��� �Ѿ��
		if (pInstance->GetMeshPath() != meshCom->GetMeshPath())
			continue;

		// Texture�� �ٸ��� ����
		if (pInstance->GetTexturePath() != meshCom->GetMaterial()->GetTexturePath())
		{
			MakeInstance(_pActor);
			return;
		}
		else
		{
			pInstance->AddInstanceMesh(meshCom);
			return;
		}
	}

	// �� ã�ƺ��� ������ ����
	MakeInstance(_pActor);
}

void ObjectManager::MakeInstance(shared_ptr<AActor> _pActor)
{
	auto pInstance = make_shared<AInstance>();

	pInstance->SetInstanceMesh(_pActor->GetMeshComponent());
	pInstance->m_bUseStencil = _pActor->m_bUseStencil;
	pInstance->Init();

	m_vInstanceList.emplace_back(pInstance);
}
