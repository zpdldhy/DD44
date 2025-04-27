#pragma once
#include "Device.h"
#include "SimpleMath.h"

enum class ProjectionType
{
	PT_ORTHOGRAPHIC,
	PT_PERSPECTIVE,
};

struct CameraConstantData
{
	Matrix matView;
	Matrix matProjection;
};

class CameraComponent
{
protected:
	ComPtr<ID3D11Buffer> m_pCameraBuffer;
	CameraConstantData m_CameraData;

	Vec3 m_vEye = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 m_vUp = Vec3(0.0f, 1.0f, 0.0f);

	// Projection
	// - Orthographic
	float m_fWidth = 800.0f;
	float m_fHeight = 600.0f;

	// - Perspective
	float m_fFov = 45.0f;
	float m_fAspect = 800.0f / 600.0f;
	float m_fNear = 0.1f;
	float m_fFar = 1000.0f;
	
public:
	virtual void Tick();
	virtual void Render(ProjectionType _projectionType);

private:
	void CreateCameraBuffer();

private:
	virtual void UpdateView() abstract;
	void UpdateOrthographicProjection();
	void UpdatePersPectiveProjection();

public:
	void SetCameraPosition(Vec3 _CameraPosition) { m_vEye = _CameraPosition; }
	void SetOrthographic(float _width, float _height);
	void SetPerspective(float _fov, float _aspect, float _near, float _far);

public:
	CameraComponent();
	virtual ~CameraComponent() {}
};

