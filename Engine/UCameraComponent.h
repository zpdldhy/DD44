#pragma once
#include "USceneComponent.h"
#include "AActor.h"
#include "Device.h"

enum class ProjectionType
{
	PT_ORTHOGRAPHIC,
	PT_PERSPECTIVE,
};

class UCameraComponent : public USceneComponent
{
	ProjectionType m_ProjectionType = ProjectionType::PT_PERSPECTIVE;

	Matrix m_matView;
	Matrix m_matProjection;

	Vec3 m_vEye;
	Vec3 m_vLook;

	// Projection
	// - Orthographic
	float m_fWidth = 800.0f;
	float m_fHeight = 600.0f;

	// - Perspective
	float m_fFov = 3.14f / 4.0f;
	float m_fAspect = 1440.0f / 900.0f;
	float m_fNear = 0.1f;
	float m_fFar = 10000.0f;
	
public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

private:
	void UpdateView();
	void UpdateOrthographicProjection();
	void UpdatePersPectiveProjection();

public:
	void SetOrthographic(float _fWidth, float _fHeight);
	void SetPerspective(float _fFov, float _fAspect, float _fNear, float _fFar);
	void SetFov(float _fFov) { m_fFov = _fFov; }
	void SetAspect(float _fAspect) { m_fAspect = _fAspect; }
	void SetNear(float _fNear) { m_fNear = _fNear; }
	void SetFar(float _fFar) { m_fFar = _fFar; }

	Matrix GetView() { return m_matView; }
	Matrix GetProejction() { return m_matProjection; }

	Vec3 GetCameraPos() { return m_vEye; }
	Vec3 GetCameraLook() { return m_vLook; }

public:
	UCameraComponent(ProjectionType _projectionType = ProjectionType::PT_PERSPECTIVE) : m_ProjectionType(_projectionType) {}
	virtual ~UCameraComponent() {}
};

