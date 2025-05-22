#include "pch.h"
#include "UIManager.h"
#include "AUIActor.h"

UINT UIManager::m_iUIID = 0;

void UIManager::Tick()
{
	for (auto pUI = m_vUIList.begin(); pUI!= m_vUIList.end();)
	{
		if (pUI->second->m_bDelete == true)
			pUI = m_vUIList.erase(pUI);
		else
		{
			pUI->second->Tick();
			m_vRenderUIList.emplace_back(pUI->second);
			pUI++;
		}
	}
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

void UIManager::AddUIList(vector<shared_ptr<class AUIActor>> _vUIList)
{
	for (auto& pUI : _vUIList)
	{
		pUI->Init();
		m_vUIList.insert(make_pair(m_iUIID, pUI));
		pUI->m_Index = m_iUIID++;
	}
}

void UIManager::AddUI(shared_ptr<class AUIActor> _pUIActor)
{
	_pUIActor->Init();
	m_vUIList.insert(make_pair(m_iUIID, _pUIActor));
	_pUIActor->m_Index = m_iUIID++;
}

shared_ptr<class AUIActor> UIManager::GetUI(UINT _iID)
{
	auto iter = m_vUIList.find(_iID);
	if (iter == m_vUIList.end())
		return nullptr;
	return iter->second;
}

vector<shared_ptr<AUIActor>> UIManager::GetUIList()
{
	vector<shared_ptr<AUIActor>> vUIList;

	for (auto& pUI : m_vUIList)
	{
		vUIList.emplace_back(pUI.second);
	}

	return vUIList;
}

void UIManager::DeleteUI(shared_ptr<class AUIActor> _pUIActor)
{
	auto iter = m_vUIList.find(_pUIActor->m_Index);
	if (iter != m_vUIList.end())
	{
		m_vUIList.erase(iter);
	}
}

void UIManager::DeleteUI(UINT _iID)
{
	auto iter = m_vUIList.find(_iID);
	if (iter != m_vUIList.end())
	{
		m_vUIList.erase(iter);
	}
}
