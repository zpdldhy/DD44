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
protected:
	bool show_demo_window = true;
	bool show_another_window = false;
	Vec4 clear_color = Vec4(0.f);

	bool m_bDark = true;
	float m_fGlowPower;
	Vec3 m_vGlowColor;
};

