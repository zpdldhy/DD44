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
	//shared_ptr<UStaticMeshResources> m_pMesh = nullptr;
	
	shared_ptr<UAnimInstance> m_pAnim = nullptr;
	int m_targetBoneIndex;
	Matrix m_matBone;
public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void PostRender() override;
	void Destroy() override;

public:
	//void SetMesh(shared_ptr<UStaticMeshResources> _mesh) { m_pMesh = _mesh; }
	const shared_ptr<UStaticMeshResources>& GetMesh() { return static_pointer_cast<UStaticMeshResources>(m_pMesh); }

	void SetAnimInstance(shared_ptr<UAnimInstance> _anim) { m_pAnim = _anim; }
	shared_ptr<UAnimInstance> GetAnimInstance() { return m_pAnim; }
	void SetTargetBoneIndex(int _index) { m_targetBoneIndex = _index; }
	int GetTargetBoneIndex() { return m_targetBoneIndex;  }
	Matrix GetMatBone() { return m_matBone; }
	void SetMatBone(Matrix mat) { m_matBone = mat; }
public:
	static shared_ptr<UStaticMeshComponent> CreateRay(Vec3 _vStart, Vec3 _vEnd);
	static shared_ptr<UStaticMeshComponent> CreateTriangle();
	static shared_ptr<UStaticMeshComponent> CreatePlane();
	static shared_ptr<UStaticMeshComponent> CreateCube();
	static shared_ptr<UStaticMeshComponent> CreateSphere(int _sliceCount, int _stackCount);

};