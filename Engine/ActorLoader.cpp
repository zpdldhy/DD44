#include "pch.h"
#include "ActorLoader.h"

#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "USkeletalMeshResources.h"
#include "UStaticMeshResources.h"

#include "UMaterial.h"
#include "Inputlayout.h"

#include "AAsset.h"

#include "APawn.h"

#include "UAnimInstance.h"
#include "AnimTrack.h"

void ActorLoader::ConvertFbxToAsset(string _path)
{
	HANDLE hFind;
	WIN32_FIND_DATAA data;
	vector<string> fileList;
	if ((hFind = FindFirstFileA(_path.c_str(), &data)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			string directory = "../Resources/Fbx/DeathDoor/";
			directory += string(data.cFileName);
			fileList.emplace_back(directory);
		} while (FindNextFileA(hFind, &data) != 0);
		FindClose(hFind);
	}

	for (int iPath = 0; iPath < fileList.size(); iPath++)
	{
		TFbxResource ret = m_FbxImporter.Load(fileList[iPath]);
		AAsset::Export(ret, fileList[iPath]);
	}
}
void ActorLoader::ConvertObjToAsset(string _path)
{
	vector<string> fileList = GetFileNames(_path);

	for (int iPath = 0; iPath < fileList.size(); iPath++)
	{
		LoadAsFbxResource(fileList[iPath]);
	}
}
void ActorLoader::LoadAsFbxResource(string path)
{
	TFbxResource ret;
	ret.Clear();
	vector<MeshData> dataList = m_ObjImporter.Load(path);
	ret.m_iMeshCount = 1;
	ret.m_vMeshList.emplace_back(dataList[0]);

	AAsset::Export(ret, path);
}

void ActorLoader::LoadAllAsset()
{
	vector<string> fileNames = GetFileNames("../Resources/Asset/*.asset");

	// TFbxResource 파싱
	for (int iPath = 0; iPath < fileNames.size(); iPath++)
	{
		TFbxResource resource = AAsset::Load(fileNames[iPath].c_str());
		m_vFbxList.emplace_back(resource);
	}
}
map<wstring, shared_ptr<UMeshResources>> ActorLoader::LoadMeshMap()
{
	Profiler p("ActorLoader::LoadMeshMap");
	m_mMeshMap.clear();
	for (int iFbx = 0; iFbx < m_vFbxList.size(); iFbx++)
	{
		for (int iMesh = 0; iMesh < m_vFbxList[iFbx].m_vMeshList.size(); iMesh++)
		{
			auto& data = m_vFbxList[iFbx].m_vMeshList[iMesh];
			shared_ptr<UMeshResources> meshResource;
			wstring name = m_vFbxList[iFbx].name + m_vFbxList[iFbx].m_vMeshList[iMesh].m_szName;
			if (data.m_bSkeleton)
			{
				meshResource = make_shared<USkeletalMeshResources>();
				meshResource->SetName(name); 
				auto skeletalMesh = dynamic_pointer_cast<USkeletalMeshResources>(meshResource);
				meshResource->SetVertexList(data.m_vVertexList);
				skeletalMesh->SetInverseBindPose(m_vFbxList[iFbx].m_vInverseBindPose[iMesh]);
				skeletalMesh->SetIwList(data.m_vIwList);
				meshResource->Create();
				// BONE
				map<wstring, BoneNode> bones;
				for (auto& data : m_vFbxList[iFbx].m_mSkeletonList)
				{
					bones.insert(make_pair(data.second.m_szName, data.second));
				}
				skeletalMesh->AddSkeleton(bones);
			}
			else
			{
				meshResource = make_shared<UStaticMeshResources>();
				meshResource->SetName(name);
				auto staticMesh = dynamic_pointer_cast<UStaticMeshResources>(meshResource);
				staticMesh->SetVertexList(data.m_vVertexList);
				if (data.m_vIndexList.size() > 0)
				{
					staticMesh->SetIndexList(data.m_vIndexList);
				}
				staticMesh->Create();
			}
			m_mMeshMap.insert(make_pair(name, meshResource));
			m_mResPathMap.insert(make_pair(m_vFbxList[iFbx].m_ResPathName, meshResource));
		}
	}
	return m_mMeshMap;
}
map<wstring, shared_ptr<UAnimInstance>> ActorLoader::LoadAnimMap()
{
	m_vAnimInstanceMap.clear();
	for (int iFbx = 0; iFbx < m_vFbxList.size(); iFbx++)
	{
		// Animation
		// Anim 없는 애들이면 걍 continue 시키자
		if (m_vFbxList[iFbx].m_iAnimTrackCount <= 0)
		{
			continue;
		}
		wstring name = m_vFbxList[iFbx].name;
		shared_ptr<UAnimInstance> animInstance = make_shared<UAnimInstance>();
		animInstance->SetBoneCount(m_vFbxList[iFbx].m_iNodeCount);
		{
			// NEW ( WITH 1DARRAY )
			for (int iAnim = 0; iAnim < m_vFbxList[iFbx].m_iAnimTrackCount; iAnim++)
			{
				AnimList animTrack;
				animTrack.m_szName = m_vFbxList[iFbx].m_vAnimTrackList[iAnim].m_szName;
				animTrack.startFrame = 0;
				animTrack.endFrame = m_vFbxList[iFbx].m_vAnimTrackList[iAnim].m_iEndFrame;
				animInstance->AddTrack(animTrack);
			}
			animInstance->MakeAnimData(m_vFbxList[iFbx].m_vAnimArray.m_vAnimList);
			//animInstance->CreateTex();
		}
		{
			// ORIGIN
			//animInstance->SetName(name);
			//animInstance->CreateConstantBuffer();
			//for (int iAnim = 0; iAnim < m_vFbxList[iFbx].m_iAnimTrackCount; iAnim++)
			//{
			//	AnimList animTrack;
			//	animTrack.m_szName = m_vFbxList[iFbx].m_vAnimTrackList[iAnim].m_szName;
			//	animTrack.animList = m_vFbxList[iFbx].m_vAnimTrackList[iAnim].m_vAnim;
			//	animInstance->AddTrack(animTrack);
			//}
		}
		m_vAnimInstanceMap.insert(make_pair(name, animInstance));
	}
	return m_vAnimInstanceMap;
}

void ActorLoader::LoadOne(string _path)
{
	m_vFbxList.clear();
	TFbxResource resource = AAsset::Load(_path.c_str());
	m_vFbxList.emplace_back(resource);
}
shared_ptr<UMeshResources> ActorLoader::LoadOneRes(string _path)
{
	auto iter = m_mResPathMap.find(to_mw(_path));
	if(iter == m_mResPathMap.end()) { assert(false); }

	return iter->second;
}
vector<MeshComponentData> ActorLoader::LoadMeshData()
{
	vector<string> fileNames = GetFileNames("../Resources/Asset/*.json");

	for (int iPath = 0; iPath < fileNames.size(); iPath++)
	{
		MeshComponentData data = AAsset::LoadJsonMesh(fileNames[iPath].c_str());
		m_vMeshDataList.emplace_back(data);
	}
	return m_vMeshDataList;
}
vector<shared_ptr<UMeshComponent>> ActorLoader::LoadMesh()
{
	//TEMP MESH TEXTUER
	shared_ptr<UMaterial> tempMat = make_shared<UMaterial>();
	wstring path = L"../Resources/Texture/magenta.png";
	tempMat->Load(path, L"../Resources/Shader/Default.hlsl");

	shared_ptr<UMaterial> tempSkinnedMat = make_shared<UMaterial>();
	tempSkinnedMat->Load(path, L"../Resources/Shader/skinningShader.hlsl");

	shared_ptr<Shader> shader = SHADER->Get(L"../Resources/Shader/skinningShader.hlsl");
	shared_ptr<Inputlayout> inputlayout = make_shared<Inputlayout>();
	INPUTLAYOUT->CreateIW(shader->m_pCode);
	tempSkinnedMat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));

	for (int iFbx = 0; iFbx < m_vFbxList.size(); iFbx++)
	{
		for (int iMesh = 0; iMesh < m_vFbxList[iFbx].m_vMeshList.size(); iMesh++)
		{
			auto& data = m_vFbxList[iFbx].m_vMeshList[iMesh];
			shared_ptr<UMeshComponent> meshComponent;
			wstring name = m_vFbxList[iFbx].name + m_vFbxList[iFbx].m_vMeshList[iMesh].m_szName;
			if (data.m_bSkeleton)
			{
				meshComponent = make_shared<USkinnedMeshComponent>();
				meshComponent->SetName(name);
				shared_ptr<USkeletalMeshResources> mesh = make_shared<USkeletalMeshResources>();
				mesh->SetName(name);
				mesh->SetVertexList(data.m_vVertexList);
				mesh->SetInverseBindPose(m_vFbxList[iFbx].m_vInverseBindPose[iMesh]);
				dynamic_cast<USkeletalMeshResources*>(mesh.get())->SetIwList(data.m_vIwList);
				mesh->Create();
				dynamic_cast<USkinnedMeshComponent*>(meshComponent.get())->SetMesh(mesh);
				meshComponent->SetMaterial(tempSkinnedMat);

				// BONE
				map<wstring, BoneNode> bones;
				for (auto& data : m_vFbxList[iFbx].m_mSkeletonList)
				{
					bones.insert(make_pair(data.second.m_szName, data.second));
				}
				mesh->AddSkeleton(bones);
			}
			else
			{
				meshComponent = make_shared<UStaticMeshComponent>();
				meshComponent->SetName(m_vFbxList[iFbx].m_vMeshList[iMesh].m_szName);
				shared_ptr<UStaticMeshResources> mesh = make_shared<UStaticMeshResources>();
				mesh->SetName(name);
				mesh->SetVertexList(data.m_vVertexList);
				mesh->Create();
				dynamic_cast<UStaticMeshComponent*>(meshComponent.get())->SetMesh(mesh);
				meshComponent->SetMaterial(tempMat);
			}
			m_vMeshList.emplace_back(meshComponent);
		}
	}
	return m_vMeshList;
}
vector<shared_ptr<UMeshResources>> ActorLoader::LoadMeshResources()
{
	for (int iFbx = 0; iFbx < m_vFbxList.size(); iFbx++)
	{
		for (int iMesh = 0; iMesh < m_vFbxList[iFbx].m_vMeshList.size(); iMesh++)
		{
			auto& data = m_vFbxList[iFbx].m_vMeshList[iMesh];
			shared_ptr<UMeshResources> meshResource;
			if (data.m_bSkeleton)
			{
				meshResource = make_shared<USkeletalMeshResources>();
				meshResource->SetName(m_vFbxList[iFbx].m_vMeshList[iMesh].m_szName);
				auto skeletalMesh = dynamic_pointer_cast<USkeletalMeshResources>(meshResource);
				meshResource->SetVertexList(data.m_vVertexList);
				skeletalMesh->SetInverseBindPose(m_vFbxList[iFbx].m_vInverseBindPose[iMesh]);
				skeletalMesh->SetIwList(data.m_vIwList);
				meshResource->Create();
				// BONE
				map<wstring, BoneNode> bones;
				for (auto& data : m_vFbxList[iFbx].m_mSkeletonList)
				{
					bones.insert(make_pair(data.second.m_szName, data.second));
				}
				skeletalMesh->AddSkeleton(bones);
			}
			else
			{
				meshResource = make_shared<UStaticMeshResources>();
				meshResource->SetName(m_vFbxList[iFbx].m_vMeshList[iMesh].m_szName);
				auto staticMesh = dynamic_pointer_cast<UStaticMeshResources>(meshResource);
				staticMesh->SetVertexList(data.m_vVertexList);
				if (data.m_vIndexList.size() > 0)
				{
					staticMesh->SetIndexList(data.m_vIndexList);
				}
				staticMesh->Create();
			}
			m_vMeshResList.emplace_back(meshResource);
		}
	}
	return m_vMeshResList;
}
vector<wstring> ActorLoader::LoadTexPath()
{
	vector<wstring> texPathList;
	for (int iFbx = 0; iFbx < m_vFbxList.size(); iFbx++)
	{
		for (int iTex = 0; iTex < m_vFbxList[iFbx].m_iTexPathCount; iTex++)
		{
			texPathList.emplace_back(m_vFbxList[iFbx].m_mTexPathList[iTex]);
		}
	}
	return texPathList;
}
vector<shared_ptr<UAnimInstance>> ActorLoader::LoadAnim()
{
	vector<shared_ptr<UAnimInstance>> animList;
	for (int iFbx = 0; iFbx < m_vFbxList.size(); iFbx++)
	{
		// Animation
		wstring name = m_vFbxList[iFbx].name;
		shared_ptr<UAnimInstance> animInstance = make_shared<UAnimInstance>();
		{
			animInstance->SetName(name);
			for (int iAnim = 0; iAnim < m_vFbxList[iFbx].m_iAnimTrackCount; iAnim++)
			{
				AnimList animTrack;
				animTrack.m_szName = m_vFbxList[iFbx].m_vAnimTrackList[iAnim].m_szName;
				animInstance->AddTrack(animTrack);
			}
		}
		animList.emplace_back(animInstance);
	}
	m_vAnimInstanceList = animList;

	return animList;
}
shared_ptr<APawn> ActorLoader::LoadOneActor(string _path)
{
	TFbxResource resource = AAsset::Load(_path.c_str());
	m_vFbxList.emplace_back(resource);

	shared_ptr<APawn> actor = make_shared<APawn>();
	shared_ptr<UMaterial> tempSkinnedMat = make_shared<UMaterial>();
	shared_ptr<UMaterial> tempMat = make_shared<UMaterial>();


	// TEXTURE
	for (auto& iter : resource.m_mTexPathList)
	{
		auto& data = iter.second;

		wstring path = L"../Resources/Texture/";
		wstring texPath = path + data;

		tempSkinnedMat->Load(texPath, L"../Resources/Shader/skinningShader.hlsl");
		shared_ptr<Shader> shader = SHADER->Get(L"../Resources/Shader/skinningShader.hlsl");
		shared_ptr<Inputlayout> inputlayout = make_shared<Inputlayout>();

		// INPUTLAYOUT 
		{
			INPUTLAYOUT->CreateIW(shader->m_pCode);
			inputlayout = INPUTLAYOUT->Get(L"IW");
		}
		tempSkinnedMat->SetInputlayout(inputlayout);

		tempMat->Load(L"../Resources/Texture/Sword.png", L"../Resources/Shader/Bloom.hlsl");
	}

	// Animation
	shared_ptr<UAnimInstance> animInstance = make_shared<UAnimInstance>();
	{
		animInstance->SetName(SplitName(to_mw(_path)));
		for (int iAnim = 0; iAnim < resource.m_iAnimTrackCount; iAnim++)
		{
			AnimList animTrack;
			animTrack.m_szName = resource.m_vAnimTrackList[iAnim].m_szName;
			animInstance->AddTrack(animTrack);
		}
		m_vAnimInstanceList.emplace_back(animInstance);
	}

	////MESH
	// TEMP
	shared_ptr<USkinnedMeshComponent> rootMesh = make_shared<USkinnedMeshComponent>();
	shared_ptr<USkeletalMeshResources> mesh = make_shared< USkeletalMeshResources>();
	mesh->SetVertexList(resource.m_vMeshList[0].m_vVertexList);
	mesh->SetIwList(resource.m_vMeshList[0].m_vIwList);
	mesh->Create();
	mesh->SetInverseBindPose(resource.m_vInverseBindPose[0]);
	rootMesh->SetMesh(mesh);
	rootMesh->SetMaterial(tempSkinnedMat);
	rootMesh->SetBaseAnim(animInstance);
	shared_ptr<AnimTrack> animTrack = make_shared< AnimTrack>();
	animTrack->SetBase(animInstance);
	rootMesh->SetMeshAnim(animTrack);
	rootMesh->SetName(resource.m_vMeshList[0].m_szName);

	{
		// BONE
		map<wstring, BoneNode> bones;
		for (auto& data : resource.m_mSkeletonList)
		{
			bones.insert(make_pair(data.second.m_szName, data.second));
		}
		mesh->AddSkeleton(bones);
	}

	//
	for (int iMesh = 1; iMesh < resource.m_vMeshList.size(); iMesh++)
	{
		shared_ptr<UMeshComponent> meshComponent;
		if (resource.m_vMeshList[iMesh].m_bSkeleton)
		{
			meshComponent = make_shared<USkinnedMeshComponent>();
			shared_ptr<USkeletalMeshResources> mesh = make_shared< USkeletalMeshResources>();
			mesh->SetVertexList(resource.m_vMeshList[iMesh].m_vVertexList);
			mesh->SetIwList(resource.m_vMeshList[iMesh].m_vIwList);
			mesh->Create();
			mesh->SetInverseBindPose(resource.m_vInverseBindPose[iMesh]);
			dynamic_cast<USkinnedMeshComponent*>(meshComponent.get())->SetMesh(mesh);
			meshComponent->SetMaterial(tempSkinnedMat);
			shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
			animTrack->SetBase(animInstance);
			dynamic_cast<USkinnedMeshComponent*>(meshComponent.get())->SetMeshAnim(animTrack);
			rootMesh->AddChild(meshComponent);
			meshComponent->SetName(resource.m_vMeshList[iMesh].m_szName);
			{
				// BONE
				map<wstring, BoneNode> bones;
				for (auto& data : resource.m_mSkeletonList)
				{
					bones.insert(make_pair(data.second.m_szName, data.second));
				}
				mesh->AddSkeleton(bones);
			}
		}
		else
		{
			meshComponent = make_shared<UStaticMeshComponent>();
			meshComponent->SetName(resource.m_vMeshList[iMesh].m_szName);
			shared_ptr<UStaticMeshResources> mesh = make_shared<UStaticMeshResources>();
			mesh->SetVertexList(resource.m_vMeshList[iMesh].m_vVertexList);
			mesh->Create();
			dynamic_cast<UStaticMeshComponent*>(meshComponent.get())->SetMesh(mesh);
			meshComponent->SetMaterial(tempMat);
		}
	}
	actor->SetMeshComponent(rootMesh);

	return actor;
}
vector<shared_ptr<APawn>> ActorLoader::LoadAllActor()
{
	vector<shared_ptr<APawn>> actorList;
	// 파일 이름 
	vector<string> fileNames = GetFileNames("../Resources/Asset/*.asset");

	for (int iPath = 0; iPath < fileNames.size(); iPath++)
	{
		actorList.emplace_back(LoadOneActor(fileNames[iPath]));
	}

	return actorList;
}
shared_ptr<APawn> ActorLoader::LoadRedeemer(string _path)
{
	TFbxResource resource = AAsset::Load(_path.c_str());
	m_vFbxList.emplace_back(resource);

	shared_ptr<APawn> actor = make_shared<APawn>();

	// Animation
	shared_ptr<UAnimInstance> animInstance = make_shared<UAnimInstance>();
	{
		animInstance->SetName(SplitName(to_mw(_path)));
		for (int iAnim = 0; iAnim < resource.m_iAnimTrackCount; iAnim++)
		{
			AnimList animTrack;
			animTrack.m_szName = resource.m_vAnimTrackList[iAnim].m_szName;
			animInstance->AddTrack(animTrack);
		}
		m_vAnimInstanceList.emplace_back(animInstance);
	}
	//
	// Material
	shared_ptr<UMaterial> rootMat = make_shared<UMaterial>();
	auto iter = resource.m_mTexPathList.find(0);
	wstring path;
	wstring p = L"../Resources/Texture/";
	if (iter == resource.m_mTexPathList.end()) { assert(false); }
	if (iter->second.empty())
	{
		path = L"../Resources/Texture/Magenta.jpg";
	}
	else
	{
		path = p + iter->second;
	}
	rootMat->Load(path, L"../Resources/Shader/skinningShader.hlsl");
	rootMat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
	////MESH
	// TEMP
	shared_ptr<USkinnedMeshComponent> rootMesh = make_shared<USkinnedMeshComponent>();
	shared_ptr<USkeletalMeshResources> mesh = make_shared< USkeletalMeshResources>();
	mesh->SetVertexList(resource.m_vMeshList[0].m_vVertexList);
	mesh->SetIwList(resource.m_vMeshList[0].m_vIwList);
	mesh->Create();
	mesh->SetInverseBindPose(resource.m_vInverseBindPose[31]);
	rootMesh->SetMesh(mesh);
	rootMesh->SetMaterial(rootMat);
	rootMesh->SetBaseAnim(animInstance);
	shared_ptr<AnimTrack> animTrack = make_shared< AnimTrack>();
	animTrack->SetBase(animInstance);
	rootMesh->SetMeshAnim(animTrack);
	rootMesh->SetName(resource.m_vMeshList[0].m_szName);
	//rootMesh->SetLocalRotation(Vec3(0.0f, -DD_PI / 2, 0.0f));

	{
		// BONE
		map<wstring, BoneNode> bones;
		for (auto& data : resource.m_mSkeletonList)
		{
			bones.insert(make_pair(data.second.m_szName, data.second));
		}
		mesh->AddSkeleton(bones);
	}

	//
	for (int iMesh = 0; iMesh < resource.m_vMeshList.size(); iMesh++)
	{
		//if (resource.m_vMeshList[iMesh].m_szName._Equal(L"Sphere")) { continue; }
		//if (resource.m_vMeshList[iMesh].m_szName._Equal(L"Plane (1)")) { continue; }
		//if (resource.m_vMeshList[iMesh].m_szName._Equal(L"Plane (2)")) { continue; }


		//if(iMesh == 31) { continue; }

		shared_ptr<UMeshComponent> meshComponent;
		if (resource.m_vMeshList[iMesh].m_bSkeleton)
		{
			meshComponent = make_shared<USkinnedMeshComponent>();
			shared_ptr<USkeletalMeshResources> mesh = make_shared< USkeletalMeshResources>();
			mesh->SetVertexList(resource.m_vMeshList[iMesh].m_vVertexList);
			mesh->SetIwList(resource.m_vMeshList[iMesh].m_vIwList);
			mesh->Create();
			mesh->SetInverseBindPose(resource.m_vInverseBindPose[iMesh]);
			dynamic_cast<USkinnedMeshComponent*>(meshComponent.get())->SetMesh(mesh);
			
			// Material
			shared_ptr<UMaterial> tempSkinnedMat = make_shared<UMaterial>();
			auto iter = resource.m_mTexPathList.find(iMesh);
			wstring path;
			if (iter == resource.m_mTexPathList.end()) { assert(false); }
			if (iter->second.empty())
			{
				path = L"../Resources/Texture/Magenta.png";
			}
			else
			{
				path = p + iter->second;
			}
			tempSkinnedMat->Load(path, L"../Resources/Shader/skinningShader.hlsl");
			tempSkinnedMat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
			meshComponent->SetMaterial(tempSkinnedMat);
			
			shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
			animTrack->SetBase(animInstance);
			dynamic_cast<USkinnedMeshComponent*>(meshComponent.get())->SetMeshAnim(animTrack);
			rootMesh->AddChild(meshComponent);
			meshComponent->SetName(resource.m_vMeshList[iMesh].m_szName);

			//meshComponent->SetLocalRotation(Vec3(0.0f, -DD_PI / 2, 0.0f));


			{
				// BONE
				map<wstring, BoneNode> bones;
				for (auto& data : resource.m_mSkeletonList)
				{
					bones.insert(make_pair(data.second.m_szName, data.second));
				}
				mesh->AddSkeleton(bones);
			}
		}
		else
		{
			meshComponent = make_shared<UStaticMeshComponent>();
			meshComponent->SetName(resource.m_vMeshList[iMesh].m_szName);
			shared_ptr<UStaticMeshResources> mesh = make_shared<UStaticMeshResources>();
			mesh->SetVertexList(resource.m_vMeshList[iMesh].m_vVertexList);
			mesh->Create();
			dynamic_cast<UStaticMeshComponent*>(meshComponent.get())->SetMesh(mesh);
			
			// Material
			shared_ptr<UMaterial> tempSkinnedMat = make_shared<UMaterial>();
			auto iter = resource.m_mTexPathList.find(iMesh);
			wstring path;
			if (iter == resource.m_mTexPathList.end()) { assert(false); }
			if (iter->second.empty())
			{
				path = L"../Resources/Texture/magenta.png";
			}
			else
			{
				path = p + iter->second;
			}
			tempSkinnedMat->Load(path, L"../Resources/Shader/Default.hlsl");
			meshComponent->SetMaterial(tempSkinnedMat);

			// ANIM
			dynamic_cast<UStaticMeshComponent*>(meshComponent.get())->SetAnimInstance(animInstance);
			int parentBone = rootMesh->GetMesh()->GetBoneIndex(L"_Root");
			Matrix matBone = animInstance->GetBoneAnim(parentBone);
			matBone = matBone.Invert();
			dynamic_cast<UStaticMeshComponent*>(meshComponent.get())->SetMatBone(matBone);
			dynamic_cast<UStaticMeshComponent*>(meshComponent.get())->SetAnimInstance(animInstance);
			dynamic_cast<UStaticMeshComponent*>(meshComponent.get())->SetTargetBoneIndex(parentBone);
			
			//meshComponent->SetLocalRotation(Vec3(-DD_PI/2, 0.0f, 0.0f));

			rootMesh->AddChild(meshComponent);
		}
	}
	actor->SetMeshComponent(rootMesh);

	return actor;
}
