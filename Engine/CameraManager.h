#pragma once
#include "Singleton.h"
#include "ConstantData.h"

enum class CameraViewType
{
	CVT_ACTOR,
	CVT_UI
};

class CameraManager : public Singleton<CameraManager>
{
	ComPtr<ID3D11Buffer> m_pCameraCB = nullptr;
	CameraConstantData m_CameraData = CameraConstantData();

	shared_ptr<class AActor> m_p3DCameraActor = nullptr;	// 3D Camera
	shared_ptr<class AActor> m_pUICameraActor = nullptr;	// Canvers
	shared_ptr<class UCameraComponent> m_p3DComponent = nullptr;
	shared_ptr<class UCameraComponent> m_pUIComponent = nullptr;

	// Frustum	
	vector<Plane> m_FrustumPlanes;
	vector<struct QuadTreeNode*> m_vCulledNodes;

public:
	void Init();
	void Tick();
	void Render(CameraViewType _ViewType);
	void Destroy();

private:
	void CreateCameraBuffer();
	void UpdateFrustumPlanes();
	void FrustumCulling(struct QuadTreeNode* pNode);
	bool CheckPointToPlane(const Vec3& _point) const;

public:
	void Set3DCameraActor(shared_ptr<class AActor> _cameraActor);	
	shared_ptr<class AActor> Get3DCameraActor();

	// Get
	shared_ptr<class UCameraComponent> Get3DCameraComponent();
	Matrix Get3DView();
	Matrix Get3DProjection();

	shared_ptr<class UCameraComponent> GetUICameraComponent();
	Matrix GetUIView();
	Matrix GetUIProjection();

	Vec3 GetNDCPos(Vec3 _vWorldPos);
	Vec3 GetScreenPos(Vec3 _vWorldPos);

	vector<Plane> GetFrustum() { return m_FrustumPlanes; }

private:
	Vec3 m_vPrevCameraPos = Vec3::Zero; // 이전 프레임 위치
	Vec3 m_vCurrentCameraPos = Vec3::Zero; // 이번 프레임 위치

public:
	Vec3 GetCameraDelta() const { return m_vCurrentCameraPos - m_vPrevCameraPos; }

};

