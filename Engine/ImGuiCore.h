#pragma once
#include "Singleton.h"
#include "MapEditorUI.h"
#include "ObjectEditorUI.h"
#include "EffectEditorUI.h"
#include "CharacterEditorUI.h"

class ImGuiCore : public Singleton<ImGuiCore>
{
private:
	std::unique_ptr<class CharacterEditorUI> m_pCharacterEditorUI;
	std::unique_ptr<class MapEditorUI> m_pMapEditorUI;
	std::unique_ptr<class ObjectEditorUI> m_pObjectEditorUI;
	std::unique_ptr<class EffectEditorUI> m_pEffectEditorUI;

public:
	bool m_bEditorToolVisible = true;

	CharacterEditorUI* GetCharacterEditorUI() const { return m_pCharacterEditorUI.get(); }
	MapEditorUI* GetMapEditorUI() const { return m_pMapEditorUI.get(); }
	ObjectEditorUI* GetObjectEditorUI() const { return m_pObjectEditorUI.get(); }
	EffectEditorUI* GetEffectEditorUI() const { return 	m_pEffectEditorUI.get(); }

	void SetCharacterEditorCallback(std::function<void(int, int, const char*, Vec3, Vec3, Vec3, int)> callback)
	{
		m_pCharacterEditorUI->SetOnCreateCallback(std::move(callback));
	}
	void SetMapEditorCallback(std::function<void()> callback)
	{
		m_pMapEditorUI->SetOnCreateCallback(std::move(callback));
	}
	void SetObjectEditorCallback(std::function<void(int actorType, int meshType, const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale)> callback)
	{
		m_pObjectEditorUI->SetOnCreateCallback(std::move(callback));
	}
	//void SetEffectEditorCallback(std::function<void(int, float, Vec3, float)> callback)
	//{
	//	//m_pEffectEditorUI->SetEffectApplyCallback(std::move(callback));
	//}

public:
	void Init();
	void Update();
	void Render();
	void Release();

protected:
	bool m_bDark = true;
};