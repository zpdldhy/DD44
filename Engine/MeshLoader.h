#pragma once
#include "SkeletalMeshData.h"

class UMeshResources;
class UMeshComponent;
class UAnimInstance;

class MeshLoader
{
	map<wstring, shared_ptr<UMeshResources>> m_mMeshMap;
	map<wstring, shared_ptr<UAnimInstance>> m_mAnimMap;
public:
	void SetMesh(map<wstring, shared_ptr<UMeshResources>> _mesh);
	void SetAnim(map<wstring, shared_ptr<UAnimInstance>> _anim);

	shared_ptr<UMeshComponent> Make(const char* _path);
	shared_ptr<UMeshComponent> Make(MeshComponentData data);

	shared_ptr<UMeshComponent> MakeMesh(MeshComponentData _data, bool _bRoot, shared_ptr<UAnimInstance> _animInstance);
	shared_ptr<UMeshComponent> MakeMesh(MeshComponentData _data, bool _bRoot);
	shared_ptr<UMeshComponent> MakeMesh(wstring _resName);
};