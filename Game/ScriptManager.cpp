#include "pch.h"
#include "ScriptManager.h"

// Character Script
#include "PlayerMoveScript.h"
#include "BatMovement.h"
#include "BettyMovement.h"
#include "WalkerMovement.h"
#include "HeadRollerMovement.h"
#include "MageMovement.h"
// Interaction Script
#include "LadderScript.h"
#include "FenceScript.h"
#include "HealerScript.h"

void ScriptManager::Init()
{
	auto script = make_shared<PlayerMoveScript>();
	m_vScriptList.insert(make_pair(L"PlayerMoveScript", script));
	m_vScriptList.insert(make_pair(L"BatMovement", make_shared<BatMovement>()));
	m_vScriptList.insert(make_pair(L"BettyMovement", make_shared<BettyMovement>()));
	m_vScriptList.insert(make_pair(L"WalkerMovement", make_shared<WalkerMovement>()));
	m_vScriptList.insert(make_pair(L"MageMovement", make_shared<MageMovement>()));
	m_vScriptList.insert(make_pair(L"HeadRollerMovement", make_shared<HeadRollerMovement>()));

	m_vScriptList.insert(make_pair(L"LadderScript", make_shared<LadderScript>()));
	m_vScriptList.insert(make_pair(L"FenceScript", make_shared<FenceScript>()));
	m_vScriptList.insert(make_pair(L"HealerScript", make_shared<HealerScript>()));
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
