#include "pch.h"
#include "UShapeComponent.h"
#include "AActor.h"
#include "AInstance.h"
#include "DxState.h"

vector<shared_ptr<class AInstance>> UShapeComponent::m_vInstanceList;
bool UShapeComponent::isRendered = false;

void UShapeComponent::Render()
{
#ifndef DEBUG
	if (isRendered == true)
		return;

	if (m_bVisible && g_bRangeVisibleMode)
	{
		if (m_pCurrentRasterizer)
			m_pCurrentRasterizer.Reset();

		DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
		DC->RSSetState(STATE->m_pRSWireFrame.Get());

		for(auto& pInstance : m_vInstanceList)
		{
			pInstance->Render();
		}

		DC->RSSetState(m_pCurrentRasterizer.Get());

		m_pCurrentRasterizer.Reset();
	}

	isRendered = true;
#endif // DEBUG
}

void UShapeComponent::SetInstance(shared_ptr<AActor> _pActor)
{
	auto meshCom = _pActor->GetMeshComponent();

	for (auto& pInstance : m_vInstanceList)
	{
		// Mesh가 다르면 넘어가요
		if (pInstance->GetMeshPath() != meshCom->GetMeshPath())
			continue;

		// Texture만 다르면 생성
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

	// 다 찾아보고 없으면 생성
	MakeInstance(_pActor);
}

void UShapeComponent::MakeInstance(shared_ptr<AActor> _pActor)
{
	auto pInstance = make_shared<AInstance>();

	pInstance->SetInstanceMesh(_pActor->GetMeshComponent());
	pInstance->m_bUseStencil = _pActor->m_bUseStencil;
	pInstance->Init();

	m_vInstanceList.emplace_back(pInstance);
}
