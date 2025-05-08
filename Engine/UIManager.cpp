#include "pch.h"
#include "UIManager.h"
#include "AUIActor.h"

void UIManager::Init()
{
	for (auto& pUIActor : m_vUIList)
		pUIActor->Init();
}

void UIManager::Tick()
{
	for (auto pUI = m_vUIList.begin();pUI!= m_vUIList.end();)
	{
		if (pUI->get()->IsDelete() == true)
			pUI = m_vUIList.erase(pUI);
		else
		{
			pUI->get()->Tick();
			pUI++;
		}
	}

	m_vRenderUIList = m_vUIList;
}

void UIManager::Render()
{
	for (auto& pUIActor : m_vRenderUIList)
		pUIActor->Render();

	m_vRenderUIList.clear();
}

void UIManager::Destroy()
{
	m_vUIList.clear();	
	m_vRenderUIList.clear();
}