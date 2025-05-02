#pragma once
#include "Singleton.h"
#include "MapEditorUI.h"
#include "ObjectEditorUI.h"
#include "EffectEditorUI.h"

class UMaterial;

class ImGuiCore : public Singleton<ImGuiCore>
{
private:
	std::unique_ptr<class MapEditorUI> m_pMapEditorUI;
	std::unique_ptr<class ObjectEditorUI> m_pObjectEditorUI;
	std::unique_ptr<class EffectEditorUI> m_pEffectEditorUI;
public:
	bool show_map_editor = true;
	bool show_object_editor = true;
	bool show_effect_editor = true;

	MapEditorUI* GetMapEditorUI() const { return m_pMapEditorUI.get(); }

	void SetMapEditorCallback(std::function<void()> callback)
	{
		m_pMapEditorUI->SetOnCreateCallback(std::move(callback));
	}


public:
	void Init();
	void Update();
	void Render();


	void Test();
public:
	float GetGlowPower() const { return m_fGlowPower; }
	Vec3 GetGlowColor() const { return m_vGlowColor; }
	float GetDissolveThreshold() const { return m_fDissolveThreshold; }
	float m_fGlowPower;
	Vec3 m_vGlowColor;
	float m_fDissolveThreshold = 0.5f;
	bool m_bShowGlowControl = true;

	int m_iSelectedActor = -1;

	bool m_bCheckbox = false;
	bool m_bDidInitialApply = false;
public:
	void SetInitialMaterialValues(const std::shared_ptr<UMaterial>& mat);

protected:
	bool show_demo_window = true;
	bool show_another_window = false;
	bool m_bDark = true;


	Vec4 clear_color = Vec4(0.f);

};

