#include "pch.h"
#include "UIManager.h"
#include "AUIActor.h"
#include "Timer.h"
#include "PrefabToActor.h"

UINT UIManager::m_iUIID = 0;

void UIManager::Tick()
{
	for (auto pUI = m_vUIList.begin(); pUI != m_vUIList.end();)
	{
		if (pUI->second->m_bDelete == true)
			pUI = m_vUIList.erase(pUI);
		else
		{
			pUI->second->Tick();
			if (pUI->second->m_bRender != false)
				m_vRenderUIList.emplace_back(pUI->second);
			pUI++;
		}
	}

	UpdateFade();
}

void UIManager::Render()
{
	for (auto& pUIActor : m_vRenderUIList)
		pUIActor->Render();

	if (m_pFade)
		m_pFade->Render();

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

void UIManager::AddUIList(vector<shared_ptr<class AActor>> _vUIList)
{
	for (auto& pUI : _vUIList)
	{
		pUI->Init();
		m_vUIList.insert(make_pair(m_iUIID, static_pointer_cast<AUIActor>(pUI)));
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

void UIManager::RemoveAll()
{
	m_vUIList.clear();
	m_vRenderUIList.clear();
	m_iUIID = 0;
}

void UIManager::UpdateFade()
{
	// 1Frame만 true 해주도록 설계
	if (!isFadeIn && !isFadeOut)
		isFadeWorkDone = false;

	if (m_pFade == nullptr)
		return;

	// 첫번째 프레임 씹기
	if (isFirstFrame)
	{
		isFirstFrame = false;
		return;
	}

	static float fCurrentFadeTime = 0.f;

	if (isFadeIn)
	{
		fCurrentFadeTime += TIMER->GetDeltaTime();
		float temp = (fCurrentFadeTime - m_fFadeTime) / m_fFadeTime;

		m_pFade->SetColor(Color(m_cFadeColor.x, m_cFadeColor.y, m_cFadeColor.z, temp));
		if (m_fFadeTime < fCurrentFadeTime)
		{
			isFadeWorkDone = true;
			isFadeIn = false;
			fCurrentFadeTime = 0.f;
		}
	}

	if (isFadeOut)
	{
		fCurrentFadeTime += TIMER->GetDeltaTime();
		float temp = -(fCurrentFadeTime / m_fFadeTime);
		m_pFade->SetColor(Color(m_cFadeColor.x, m_cFadeColor.y, m_cFadeColor.z, temp));

		if (m_fFadeTime < fCurrentFadeTime)
		{
			isFadeWorkDone = true;
			isFadeOut = false;
			fCurrentFadeTime = 0.f;
			m_pFade = nullptr;
		}
	}
	if (m_pFade)
		m_pFade->Tick();
}

void UIManager::SetFadeActor(shared_ptr<AUIActor> _pFade)
{
	if (_pFade == nullptr)
		return;

	m_pFade = _pFade;
	m_pFade->Init();
}
