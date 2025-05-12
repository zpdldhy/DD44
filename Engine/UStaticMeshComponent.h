#pragma once
#include "UMeshComponent.h"
#include "UStaticMeshResources.h"
#include "UAnimInstance.h"

class UStaticMeshComponent : public UMeshComponent
{
public:
	UStaticMeshComponent() = default;
	virtual ~UStaticMeshComponent() = default;

protected:
	shared_ptr<UStaticMeshResources> m_pMesh = nullptr;
	shared_ptr<UAnimInstance> m_pAnim = nullptr;

	int targetBoneIndex;
public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void PostRender() override;
	void Destroy() override;

public:
	void SetMesh(shared_ptr<UStaticMeshResources> _mesh) { m_pMesh = _mesh; }
	shared_ptr<UStaticMeshResources> GetMesh() { return m_pMesh; }

	void SetAnimInstance(shared_ptr<UAnimInstance> _anim) { m_pAnim = _anim; }
	void SetTargetBoneIndex(int _index) { targetBoneIndex = _index; }
public:
	static shared_ptr<UStaticMeshComponent> CreateRay();
	static shared_ptr<UStaticMeshComponent> CreateTriangle();
	static shared_ptr<UStaticMeshComponent> CreatePlane();
	static shared_ptr<UStaticMeshComponent> CreateCube();
	static shared_ptr<UStaticMeshComponent> CreateSphere(int _sliceCount, int _stackCount);

	//TEMP Áö¿ï°Í
	Matrix matBone;
	void SetMatBone(Matrix mat) { matBone = mat; }
};