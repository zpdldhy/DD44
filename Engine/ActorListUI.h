#pragma once

class ActorListUI
{
public:
	void DrawUI();
	void Toggle();

private:
	bool m_bVisible = false;
	float m_fSlideOffset = 0.0f;

	Vec3 m_vPosition;
	Vec3 m_vRotation;
	Vec3 m_vScale;
	int m_iLastActorID = -1;
	int m_iSelectedActorID = -1;

};

