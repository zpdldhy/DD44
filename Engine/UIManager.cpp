#include "pch.h"
#include "UIManager.h"
#include "AUIActor.h"

void UIManager::Init()
{
}

void UIManager::Render()
{
	for (auto& pUIActor : m_vUIList)
		pUIActor->Render();

	m_vUIList.clear();
}

void UIManager::Destroy()
{
	m_vUIList.clear();	
}
