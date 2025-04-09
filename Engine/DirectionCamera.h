#pragma once
#include "Camera.h"

class DirectionCamera : public Camera
{
protected:
	Vec3 m_vLook = Vec3(0.0f, 0.0f, 1.0f);

private:
	void UpdateView() override;

public:
	void SetLook(Vec3 _look) { m_vLook = _look; }
};

