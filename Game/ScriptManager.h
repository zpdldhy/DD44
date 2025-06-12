#pragma once
#include "Singleton.h"
#include "UScriptComponent.h"

class ScriptManager : public Singleton<ScriptManager>
{
	map<wstring, shared_ptr<UScriptComponent>> m_vScriptList;
public:
	void Init();
public:
	shared_ptr<UScriptComponent> GetScript(wstring _name);
};

