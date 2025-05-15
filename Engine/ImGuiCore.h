#pragma once
#include "Singleton.h"
#include "MapEditorUI.h"
#include "ObjectEditorUI.h"
#include "EffectEditorUI.h"
#include "CharacterEditorUI.h"
#include "ActorListUI.h"
#include "MeshEditorUI.h"

class ImGuiCore : public Singleton<ImGuiCore>
{
private:
	std::unique_ptr<class CharacterEditorUI> m_pCharacterEditorUI;
	std::unique_ptr<class MapEditorUI> m_pMapEditorUI;
	std::unique_ptr<class ObjectEditorUI> m_pObjectEditorUI;
	std::unique_ptr<class EffectEditorUI> m_pEffectEditorUI;
	std::unique_ptr<class ActorListUI> m_pActorListUI;
	std::unique_ptr<class MeshEditorUI> m_pMeshEditorUI;

public:
	bool m_bEditorToolVisible = true;

	CharacterEditorUI* GetCharacterEditorUI() const { return m_pCharacterEditorUI.get(); }
	MapEditorUI* GetMapEditorUI() const { return m_pMapEditorUI.get(); }
	ObjectEditorUI* GetObjectEditorUI() const { return m_pObjectEditorUI.get(); }
	EffectEditorUI* GetEffectEditorUI() const { return 	m_pEffectEditorUI.get(); }
	MeshEditorUI* GetMeshEditorUI() const { return m_pMeshEditorUI.get(); }

	void SetCharacterEditorCallback(std::function<void(std::shared_ptr<UMeshComponent>, const Vec3&, const Vec3&, const Vec3&, int) > callback)
	{
		m_pCharacterEditorUI->SetOnCreateCallback(std::move(callback));
	}
	void SetMapEditorCallback(std::function<void()> callback)
	{
		m_pMapEditorUI->SetOnCreateCallback(std::move(callback));
	}
	void SetObjectEditorCallback(std::function<void(const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale)> callback)
	{
		m_pObjectEditorUI->SetOnCreateCallback(std::move(callback));
	}
	void SetEffectEditorCallback(std::function<void(int, float, Vec3, float, Vec3, float)> callback)
	{
		m_pEffectEditorUI->SetEffectApplyCallback(std::move(callback));
	}
	void SetLightEditorCallback(std::function<void(ELightType, Vec3, float, Vec3, float)> callback)
	{
		m_pEffectEditorUI->SetLightApplyCallback(std::move(callback));
	}
	void SetBaseMeshEditorCallback(std::function<void(int, int, int, shared_ptr<APawn>&)> callback)
	{
		m_pMeshEditorUI->CreateActorCallback(std::move(callback));
	}
	void SetChildMeshEditorCallback(std::function<void(ChildMeshData)> callback)
	{
		m_pMeshEditorUI->CreateChildMeshCallback(std::move(callback));
	}

public:
	void Init();
	void Update();
	void Render();
	void Release();

protected:
	bool m_bDark = true;
};