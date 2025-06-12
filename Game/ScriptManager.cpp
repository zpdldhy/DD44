#include "pch.h"
#include "ScriptManager.h"

// Character Script
#include "PlayerMoveScript.h"
#include "BatMovement.h"
#include "BettyMovement.h"
#include "WalkerMovement.h"
#include "MageMovement.h"

void ScriptManager::Init()
{
	auto script = make_shared<PlayerMoveScript>();
	m_vScriptList.insert(make_pair(L"PlayerMoveScript", script));
	m_vScriptList.insert(make_pair(L"BatMovement", make_shared<BatMovement>()));
	m_vScriptList.insert(make_pair(L"BettyMovement", make_shared<BettyMovement>()));
	m_vScriptList.insert(make_pair(L"WalkerMovement", make_shared<WalkerMovement>()));
	m_vScriptList.insert(make_pair(L"MageMovement", make_shared<MageMovement>()));

}

shared_ptr<UScriptComponent> ScriptManager::GetScript(wstring _name)
{
	auto script = m_vScriptList.find(_name);
	if (script  == m_vScriptList.end() || !script->second)
	{
		assert(false);
	}

	return script->second->Clone();
}
