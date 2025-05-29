#include "pch.h"
#include "ObjectManager.h"
#include "AActor.h"
#include "UMeshComponent.h"
#include "UMaterial.h"
#include "DxState.h"
#include "ATerrainTileActor.h"

UINT ObjectManager::ActorCount = 0;
ComPtr<ID3D11Buffer> ObjectManager::m_pRenderModeBuffer = nullptr;

void ObjectManager::Init()
{
	CreateRenderModeCB();
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
			pActor->second->Tick();
			m_vRenderActorList.emplace_back(pActor->second);	// �ӽ� ���.
			pActor++;
		}		
	}
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

	ClearRenderList();
}

void ObjectManager::Destroy()
{
	m_vActorList.clear();
	ClearRenderList();
	ActorCount = 0;
}

void ObjectManager::AddActor(shared_ptr<class AActor> _pActor)
{
	_pActor->m_Index = ActorCount;
	_pActor->Init();
	m_vActorList.insert(make_pair(ActorCount, _pActor));
	m_vActorIndexList.emplace_back(ActorCount);		// �ӽ� ���

	ActorCount++;
}

void ObjectManager::AddActorList(vector<shared_ptr<class AActor>> _vActorList)
{
	for (auto& pActor : _vActorList)
	{
		pActor->m_Index = ActorCount;
		pActor->Init();
		m_vActorList.insert(make_pair(ActorCount, pActor));

		m_vActorIndexList.emplace_back(ActorCount);	// �ӽ� ���
		ActorCount++;
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

void ObjectManager::CheckStencilList()
{
	for (auto& pActor : m_vRenderActorList)
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
	m_vRenderActorList.clear();
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
