#include "pch.h"
#include "MeshLoader.h"
#include "UInstanceSkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "UMeshResources.h"
#include "AnimTrack.h"
#include "UAnimInstance.h"
#include "UMaterial.h"
#include "AAsset.h"

void MeshLoader::SetMesh(map<wstring, shared_ptr<UMeshResources>> _mesh)
{
	m_mMeshMap.clear();
	m_mMeshMap = _mesh;
}

void MeshLoader::SetAnim(map<wstring, shared_ptr<UAnimInstance>> _anim)
{
	m_mAnimMap.clear();
	m_mAnimMap = _anim;
}

shared_ptr<UMeshComponent> MeshLoader::Make(const char* _path)
{
	//Profiler p("MeshLoader::Make");
	auto meshData = AAsset::LoadJsonMesh(_path);
	shared_ptr<UMeshComponent> m_pRootComponent;
	auto originAnim = m_mAnimMap.find(meshData.m_szAnim);
	if (originAnim == m_mAnimMap.end())
	{
		m_pRootComponent = MakeMeshRecursive(meshData, true);
	}
	else
	{
		auto animInstance = originAnim->second->Clone();
		m_pRootComponent = MakeMeshRecursive(meshData, true, animInstance);
	}

	return m_pRootComponent;
}

shared_ptr<UMeshComponent> MeshLoader::Make(MeshComponentData data)
{
	shared_ptr<UMeshComponent> m_pRootComponent;
	auto originAnim = m_mAnimMap.find(data.m_szAnim);
	if (originAnim == m_mAnimMap.end())
	{
		m_pRootComponent = MakeMeshRecursive(data, true);
	}
	else
	{
		auto animInstance = originAnim->second->Clone();
		m_pRootComponent = MakeMeshRecursive(data, true, animInstance);
	}

	return m_pRootComponent;
}

shared_ptr<UMeshComponent> MeshLoader::MakeMeshRecursive(MeshComponentData _data, bool _bRoot, shared_ptr<UAnimInstance> _animInstance)
{
	// parameter
	shared_ptr<UMeshComponent> mesh = MakeMesh(_data, _bRoot, _animInstance);

	// child
	for (int i = 0; i < _data.m_vChild.size(); i++)
	{
		auto child = MakeMeshRecursive(_data.m_vChild[i], false, _animInstance);
		mesh->AddChild(child);
		child->SetParentTransform(dynamic_pointer_cast<UMeshComponent>(mesh).get());
	}

	return mesh;
}

shared_ptr<UMeshComponent> MeshLoader::MakeMeshRecursive(MeshComponentData _data, bool _bRoot)
{
	// parameter
	shared_ptr<UMeshComponent> mesh = MakeMesh(_data, _bRoot);

	// child
	for (int i = 0; i < _data.m_vChild.size(); i++)
	{
		auto child = MakeMeshRecursive(_data.m_vChild[i], false);
		mesh->AddChild(child);
		child->SetParentTransform(dynamic_pointer_cast<UMeshComponent>(mesh).get());
	}

	return mesh;
}

shared_ptr<UMeshComponent> MeshLoader::MakeMesh(MeshComponentData data, bool bRoot, shared_ptr<UAnimInstance> animInstance)
{
	shared_ptr<UMeshComponent> mesh;

	auto meshRes = m_mMeshMap.find(data.m_szRes);
	if (meshRes == m_mMeshMap.end()) { assert(false); }


	if (data.m_type == (int)MeshType::M_SKINNED)
	{
		mesh = make_shared<UInstanceSkinnedMeshComponent>();

		mesh->SetMesh(meshRes->second);
		{
			auto skinnedRoot = dynamic_pointer_cast<UInstanceSkinnedMeshComponent>(mesh);
			if (bRoot) { skinnedRoot->SetBaseAnim(animInstance); }
			shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
			animTrack->SetBase(animInstance);
			skinnedRoot->SetMeshAnim(animTrack);
			if (data.m_bInPlace)
			{
				animInstance->m_bInPlace = true;
				animInstance->SetRootIndex(data.m_rootIndex);
			}
		}

		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		mat->Load(data.m_szTex, data.m_szShader);
		mat->SetInputlayout(INPUTLAYOUT->Get(L"InstanceIW"));
		mesh->SetMaterial(mat);
	}
	else
	{
		mesh = make_shared<UStaticMeshComponent>();
		mesh->SetMesh(meshRes->second);

		if (data.m_bHasAnim)
		{
			auto staticRoot = dynamic_pointer_cast<UStaticMeshComponent>(mesh);
			staticRoot->SetAnimInstance(animInstance);
			staticRoot->SetMatBone(data.m_matBone);
			staticRoot->SetTargetBoneIndex(data.m_targetBone);
		}
		shared_ptr<UMaterial> mat = make_shared<UMaterial>();

		mat->Load(data.m_szTex, data.m_szShader);
		mesh->SetMaterial(mat);
	}

	mesh->SetLocalPosition(data.m_pos);
	mesh->SetLocalRotation(data.m_rot);
	mesh->SetLocalScale(data.m_scale);

	mesh->SetName(data.m_szComp);
	mesh->SetVisible(data.m_bVisible);

	return mesh;
}

shared_ptr<UMeshComponent> MeshLoader::MakeMesh(MeshComponentData data, bool _bRoot)
{
	shared_ptr<UMeshComponent> mesh;
	auto meshRes = m_mMeshMap.find(data.m_szRes);
	if (meshRes == m_mMeshMap.end()) { assert(false); }


	if (data.m_type == (int)MeshType::M_SKINNED)
	{
		mesh = make_shared<USkinnedMeshComponent>();
		//auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(mesh);
		mesh->SetMesh(meshRes->second);
		//skinnedRoot->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(meshRes->second));

		//shared_ptr<MeshTransform> transform = make_shared<MeshTransform>();
		//mesh->SetMeshTransform(transform);

		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		mat->Load(data.m_szTex, data.m_szShader);
		mat->SetInputlayout(INPUTLAYOUT->Get(L"InstanceIW"));
		mesh->SetMaterial(mat);
	}
	else
	{
		mesh = make_shared<UStaticMeshComponent>();
		auto staticRoot = dynamic_pointer_cast<UStaticMeshComponent>(mesh);
		staticRoot->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(meshRes->second));

		//shared_ptr<MeshTransform> transform = make_shared<MeshTransform>();
		//mesh->SetMeshTransform(transform);
		staticRoot->SetMatBone(data.m_matBone);

		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		mat->Load(data.m_szTex, data.m_szShader);
		staticRoot->SetMaterial(mat);
	}
	mesh->SetLocalPosition(data.m_pos);
	mesh->SetLocalRotation(data.m_rot);
	mesh->SetLocalScale(data.m_scale);

	mesh->SetName(data.m_szComp);
	mesh->SetVisible(data.m_bVisible);

	return mesh;
}

shared_ptr<UMeshComponent> MeshLoader::MakeMesh(wstring _resName)
{
	shared_ptr<UMeshComponent> mesh;
	auto iter = m_mMeshMap.find(_resName);
	if (iter == m_mMeshMap.end()) { assert(false); }


	bool skinned = dynamic_pointer_cast<USkeletalMeshResources>(mesh) ? true : false;
	if (skinned)
	{
		mesh = make_shared<USkinnedMeshComponent>();
		//auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(mesh);
		mesh->SetMesh(iter->second);

		//shared_ptr<MeshTransform> transform = make_shared<MeshTransform>();
		//mesh->SetMeshTransform(transform);
	}
	else
	{
		mesh = make_shared<UStaticMeshComponent>();
		auto staticRoot = dynamic_pointer_cast<UStaticMeshComponent>(mesh);
		staticRoot->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(iter->second));

		//shared_ptr<MeshTransform> transform = make_shared<MeshTransform>();
		//mesh->SetMeshTransform(transform);
	}
	return mesh;
}

shared_ptr<UMeshResources> MeshLoader::GetMeshRes(wstring _name)
{
	auto res = m_mMeshMap.find(_name);
	if (res != m_mMeshMap.end())
	{
		return res->second;
	}
	else
	{
		assert(false);
		//return shared_ptr<UMeshResources>();
	}
	return nullptr;
}

shared_ptr<UAnimInstance> MeshLoader::GetAnimInstance(wstring _name)
{
	auto res = m_mAnimMap.find(_name);
	if (res != m_mAnimMap.end())
	{
		return res->second;
	}
	else
	{
		assert(false);
		//return shared_ptr<UMeshResources>();
	}
	return nullptr;
}
