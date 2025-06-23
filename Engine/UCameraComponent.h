#pragma once
#include "USceneComponent.h"
#include "AActor.h"
#include "Device.h"

enum class ViewType
{
	VT_LOOKAT,
	VT_LOOKTO,
};

enum class ProjectionType
{
	PT_ORTHOGRAPHIC,
	PT_PERSPECTIVE,
};

struct FrustumData
{
	Matrix matViewProjInvert;
};

class UCameraComponent : public USceneComponent
{
private:
	FrustumData m_tFrustumData = { {Matrix::Identity } };
	static ComPtr<ID3D11Buffer> m_pFrustumCB;

protected:
	ProjectionType m_ProjectionType = ProjectionType::PT_PERSPECTIVE;
	ViewType	   m_ViewType = ViewType::VT_LOOKTO;

	Matrix m_matView = Matrix::Identity;
	Matrix m_matProjection = Matrix::Identity;

	// View
	Vec3 m_vLookAt = { 0.f, 0.f, 1.f };
	Vec3 m_vLookTo = { 0.f, 0.f, 1.f };

	// Projection
	// - Orthographic
	float m_fWidth = static_cast<float>(g_windowSize.x);
	float m_fHeight = static_cast<float>(g_windowSize.y);

	// - Perspective
	float m_fFov = 3.14f / 4.0f;
	float m_fAspect = static_cast<float>(g_windowSize.x) / static_cast<float>(g_windowSize.y);
	float m_fNear = 0.1f;
	float m_fFar = 5000.0f;

	// Frustum
	shared_ptr<class AActor> m_pFrustumBox = nullptr;
	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;
	bool m_bVisibleFrustumBox = true;
	
public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

private:
	void CreateFrustumBox();
	void CreateFrustumBuffer();
	void UpdateFrustumBox();
	void UpdateFrustumBuffer();

	void UpdateView();
	void UpdateProjection();

public:
	void SetLookAt(Vec3 _FocusPosition) { m_ViewType = ViewType::VT_LOOKAT; m_vLookAt = _FocusPosition; }
	void SetLookTo(Vec3 _Direction) { m_ViewType = ViewType::VT_LOOKTO; m_vLookTo = _Direction; }

public:
	void SetOrthographic(float _fWidth, float _fHeight);
	void SetPerspective(float _fFov, float _fAspect, float _fNear, float _fFar);
	void SetFov(float _fFov) { m_fFov = _fFov; }
	void SetAspect(float _fAspect) { m_fAspect = _fAspect; }
	void SetNear(float _fNear) { m_fNear = _fNear; }
	void SetFar(float _fFar) { m_fFar = _fFar; }
	void SetFrustumVisible(bool _bVisible) { m_bVisibleFrustumBox = _bVisible; }

	Matrix GetView() { return m_matView; }
	Matrix GetProjection() { return m_matProjection; }
	Matrix GetViewProjection() { return m_matView * m_matProjection; }

	Vec3 GetCameraLook() { return m_vLookTo; }


public:
	UCameraComponent(ProjectionType _projectionType = ProjectionType::PT_PERSPECTIVE) : m_ProjectionType(_projectionType) {}
	virtual ~UCameraComponent() {}
};

