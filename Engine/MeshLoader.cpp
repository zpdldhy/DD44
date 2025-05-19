#include "pch.h"
#include "MeshLoader.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "UMeshResources.h"
#include "AnimTrack.h"
#include "UAnimInstance.h"
#include "UMaterial.h"
#include "AAsset.h"

void MeshLoader::SetMesh(map<wstring, shared_ptr<UMeshResources>> _mesh)
{
	m_mMeshMap = _mesh;
}

void MeshLoader::SetAnim(map<wstring, shared_ptr<UAnimInstance>> _anim)
{
	m_mAnimMap = _anim;
}

shared_ptr<UMeshComponent> MeshLoader::Make(const char*_path)
{
	auto meshData = AAsset::LoadMesh(_path);

	auto originAnim = m_mAnimMap.find(meshData.m_szAnim);
	if (originAnim == m_mAnimMap.end()) { assert(false); }
	auto animInstance = originAnim->second->Clone();

	shared_ptr<UMeshComponent> m_pRootComponent = MakeMesh(meshData, true, animInstance);

	auto& data = meshData;
	for (int i = 0; i < data.m_vChild.size(); i++)
	{
		m_pRootComponent->AddChild(MakeMesh(data.m_vChild[i], false, animInstance));
	}

	return m_pRootComponent;
}

shared_ptr<UMeshComponent> MeshLoader::MakeMesh(MeshComponentData data, bool bRoot, shared_ptr<UAnimInstance> animInstance)
{
	shared_ptr<UMeshComponent> mesh;
	auto meshRes = m_mMeshMap.find(data.m_szRes);
	if (meshRes == m_mMeshMap.end()) { assert(false); }


	if (data.m_type == (int)MeshType::M_SKINNED)
	{
		mesh = make_shared<USkinnedMeshComponent>();
		auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(mesh);
		skinnedRoot->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(meshRes->second));

		if (bRoot) { skinnedRoot->SetBaseAnim(animInstance); }
		shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
		animTrack->SetBase(animInstance);
		skinnedRoot->SetMeshAnim(animTrack);
		if (data.m_bInPlace)
		{
			animInstance->m_bInPlace = true;
			animInstance->SetRootIndex(data.m_rootIndex);
		}

		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		mat->Load(data.m_szTex, data.m_szShader);
		mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
		skinnedRoot->SetMaterial(mat);
	}
	else
	{
		mesh = make_shared<UStaticMeshComponent>();
		auto staticRoot = dynamic_pointer_cast<UStaticMeshComponent>(mesh);
		staticRoot->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(meshRes->second));

		staticRoot->SetAnimInstance(animInstance);
		staticRoot->SetMatBone(data.m_matBone);
		staticRoot->SetTargetBoneIndex(data.m_targetBone);

		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		mat->Load(data.m_szTex, data.m_szShader);
		staticRoot->SetMaterial(mat);
	}
	mesh->SetLocalPosition(data.m_pos);
	mesh->SetLocalRotation(data.m_rot);
	mesh->SetLocalScale(data.m_scale);

	return mesh;
}
