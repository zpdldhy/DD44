#include "pch.h"
#include "MeshLoader.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "UMeshResources.h"
#include "AnimTrack.h"
#include "UAnimInstance.h"
#include "UMaterial.h"
#include "AAsset.h"
#include "MeshTransform.h"

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
		m_pRootComponent = MakeMesh(meshData, true);
		auto& data = meshData;
		for (int i = 0; i < data.m_vChild.size(); i++)
		{
			m_pRootComponent->AddChild(MakeMesh(data.m_vChild[i], false));
		}
	}
	else
	{
		auto animInstance = originAnim->second->Clone();
		m_pRootComponent = MakeMesh(meshData, true, animInstance);
		auto& data = meshData;
		for (int i = 0; i < data.m_vChild.size(); i++)
		{
			m_pRootComponent->AddChild(MakeMesh(data.m_vChild[i], false, animInstance));
		}
	}

	return m_pRootComponent;
}

shared_ptr<UMeshComponent> MeshLoader::Make(MeshComponentData data)
{
	shared_ptr<UMeshComponent> m_pRootComponent;
	auto originAnim = m_mAnimMap.find(data.m_szAnim);
	if (originAnim == m_mAnimMap.end())
	{
		m_pRootComponent = MakeMesh(data, true);
		for (int i = 0; i < data.m_vChild.size(); i++)
		{
			m_pRootComponent->AddChild(MakeMesh(data.m_vChild[i], false));
		}
	}
	else
	{
		auto animInstance = originAnim->second->Clone();
		m_pRootComponent = MakeMesh(data, true, animInstance);
		for (int i = 0; i < data.m_vChild.size(); i++)
		{
			m_pRootComponent->AddChild(MakeMesh(data.m_vChild[i], false, animInstance));
		}
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
		//Profiler p("MeshLoader::MakeMeshSkinned");
		mesh = make_shared<USkinnedMeshComponent>();
		auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(mesh);
		skinnedRoot->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(meshRes->second));
		//shared_ptr<MeshTransform> transform = make_shared<MeshTransform>();
		//mesh->SetMeshTransform(transform);
		{
			//Profiler p("Set Anim");

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

		{
			//Profiler p("Skinned : SetMat");
			shared_ptr<UMaterial> mat = make_shared<UMaterial>();
			{
				//Profiler p("MatLoad");

				mat->Load(data.m_szTex, data.m_szShader);

			}
			{
				//Profiler p("Inputlayout");

				mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));

			}
			skinnedRoot->SetMaterial(mat);

		}
		int a = 0;

	}
	else
	{
		mesh = make_shared<UStaticMeshComponent>();
		auto staticRoot = dynamic_pointer_cast<UStaticMeshComponent>(mesh);
		staticRoot->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(meshRes->second));

		//shared_ptr<MeshTransform> transform = make_shared<MeshTransform>();
		//mesh->SetMeshTransform(transform);
		

		staticRoot->SetAnimInstance(animInstance);
		staticRoot->SetMatBone(data.m_matBone);
		staticRoot->SetTargetBoneIndex(data.m_targetBone);

		{
			//Profiler p("StaticMesh : SetMat");
			shared_ptr<UMaterial> mat = make_shared<UMaterial>();
			{
				//Profiler p("MatLoad");

				mat->Load(data.m_szTex, data.m_szShader);

			}
			staticRoot->SetMaterial(mat);
		}
	}
	mesh->SetLocalPosition(data.m_pos);
	mesh->SetLocalRotation(data.m_rot);
	mesh->SetLocalScale(data.m_scale);

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
		auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(mesh);
		skinnedRoot->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(meshRes->second));

		//shared_ptr<MeshTransform> transform = make_shared<MeshTransform>();
		//mesh->SetMeshTransform(transform);

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
		auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(mesh);
		skinnedRoot->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(iter->second));

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
}
