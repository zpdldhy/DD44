#include "pch.h"
#include "UIManager.h"
#include "AUIActor.h"

void UIManager::Tick()
{
	for (auto pUI = m_vUIList.begin();pUI!= m_vUIList.end();)
	{
		if (pUI->get()->m_bDelete == true)
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

void UIManager::SetUIList(vector<shared_ptr<class AUIActor>> _vUIList)
{
	for (auto& pUI : _vUIList)
		pUI->Init();
	m_vUIList = _vUIList;
}

void UIManager::AddUIList(vector<shared_ptr<class AUIActor>> _vUIList)
{
	for (auto& pUI : _vUIList)
	{
		pUI->Init();
		m_vUIList.emplace_back(pUI);
	}
}

void UIManager::AddUI(shared_ptr<class AUIActor> _pUIActor)
{
	_pUIActor->Init();
	m_vUIList.emplace_back(_pUIActor);
}