#pragma once

class MousePicking :  public Ray
{
public:
	POINT m_pMouseWinPos = { 0, 0 };
	Vec3 m_vMouseEndPos = { 0.f, 0.f, 1.f };

public:
	void Click();
};

