#pragma once
#include "Camera.h"

class TargetCamera : public Camera
{
protected:
	Vec3 m_vTarget = Vec3(0.0f, 0.0f, 0.0f);

private:
    void UpdateView() override;

public:
	void SetTarget(Vec3 _target) { m_vTarget = _target; }
	Vec3 GetTarget() { return m_vTarget; }
};

