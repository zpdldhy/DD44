#pragma once
#include "USceneComponent.h"
#include "AActor.h"
#include "Device.h"

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

class UCameraComponent : public USceneComponent
{
	ProjectionType m_ProjectionType = ProjectionType::PT_PERSPECTIVE;

	ComPtr<ID3D11Buffer> m_pCameraCB;
	CameraConstantData m_CameraData;

	// Projection
	// - Orthographic
	float m_fWidth = 800.0f;
	float m_fHeight = 600.0f;

	// - Perspective
	float m_fFov = 3.14f / 4.0f;
	float m_fAspect = 800.0f / 600.0f;
	float m_fNear = 0.1f;
	float m_fFar = 10000.0f;
	
public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

private:
	void CreateCameraBuffer();

private:
	void UpdateView();
	void UpdateOrthographicProjection();
	void UpdatePersPectiveProjection();

public:
	//void SetCameraPosition(Vec3 _CameraPosition) { m_vEye = _CameraPosition; }
	void SetOrthographic(float _width, float _height);
	void SetPerspective(float _fov, float _aspect, float _near, float _far);

public:
	UCameraComponent(ProjectionType _projectionType) : m_ProjectionType(_projectionType) {}
	virtual ~UCameraComponent() {}
};

