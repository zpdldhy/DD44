#pragma once
#include "AActor.h"
#include "Device.h"
#include "SimpleMath.h"
#include <DirectXMath.h>

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

class CameraActor :public AActor
{
protected:
	// Default Projection - Perspective
	ProjectionType m_projectionType = ProjectionType::PT_PERSPECTIVE;

	CameraConstantData m_cameraData;
	Matrix m_matView = Matrix::Identity;
	Matrix m_matOrthographicProjection = Matrix::Identity;
	Matrix m_matPerspectiveProjection = Matrix::Identity;

	// Projection Data
	// - Orthographic
	float m_fWidth = 800.0f;
	float m_fHeight = 600.0f;
	// - Perspective
	float m_fFov = (float)DirectX::XM_PI / 4.0f;
	float m_fAspect = 600.0f / 800.0f;
	float m_fNear = 0.1f;
	float m_fFar = 1000.0f;

public:
	void Tick() override;
	void PreRender() override;
	void PostRender() override;

private:
	bool CreateConstantBuffer() override;

private:	
	void UpdateViewMatrix();
	void UpdateOrthographicProjection();
	void UpdatePersPectiveProjection();

public:
	void SetProjectionType(ProjectionType _type) { m_projectionType = _type; }
	ProjectionType GetProjectionType() { return m_projectionType; }
	void SetOrthographic(float _width, float _height);
	void SetPerspective(float _fov, float _aspect, float _near, float _far);

public:
	CameraActor() {}
	virtual ~CameraActor() {}
};

