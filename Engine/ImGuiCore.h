#pragma once
#include "Singleton.h"

class UMaterial;

class ImGuiCore : public Singleton<ImGuiCore>
{

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
	Vec4 clear_color = Vec4(0.f);

	bool m_bDark = true;


};

