#pragma once
#include "Singleton.h"

class ImGuiCore : public Singleton<ImGuiCore>
{
public:
	void Init();
	void Update();
	void Render();

	void Test();
protected:
	bool show_demo_window = true;
	bool show_another_window = false;
	Vec4 clear_color = Vec4(0.f);

	bool m_bDark = true;
};

