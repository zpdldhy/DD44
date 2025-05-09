#include "pch.h"
#include "ActorLoader.h"
#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "UMaterial.h"
#include "AAsset.h"
#include "APawn.h"
#include "AnimTrack.h"
#include "USkeletalMeshResources.h"
#include "UStaticMeshResources.h"
#include "Inputlayout.h"

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

vector<shared_ptr<AActor>> ActorLoader::Load()
{
	vector<shared_ptr<AActor>> actorList;
	// ���� �̸� 
	vector<string> fileNames = GetFileNames("../Resources/Asset/*.asset");

	// TFbxResource �Ľ�

	for (int iPath = 0; iPath < fileNames.size(); iPath++)
	{
		actorList.emplace_back(LoadOne(fileNames[iPath]));
	}
	return actorList;
}

shared_ptr<APawn> ActorLoader::LoadOne(string _path)
{
	TFbxResource resource = AAsset::Load(_path.c_str());

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
		
		tempMat->Load(texPath, L"../Resources/Shader/Default.hlsl");
	}

	// Animation
	shared_ptr<UAnimInstance> animInstance = make_shared<UAnimInstance>();
	{
		animInstance->SetName(SplitName(to_mw(_path)));
		animInstance->CreateConstantBuffer();
		for (int iAnim = 0; iAnim < resource.m_iAnimTrackCount; iAnim++)
		{
			AnimList animTrack;
			animTrack.m_szName = resource.m_vAnimTrackList[iAnim].m_szName;
			animTrack.animList = resource.m_vAnimTrackList[iAnim].m_vAnim;
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
	m_vMeshList.emplace_back(rootMesh);
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
		m_vMeshList.emplace_back(meshComponent);
	}
	m_vFbxList.emplace_back(resource);
	actor->SetMeshComponent(rootMesh);

	return actor;
}

vector<shared_ptr<UMeshComponent>> ActorLoader::LoadMesh()
{
	// TEMP MESH TEXTUER
	//shared_ptr<UMaterial> tempMat = make_shared<UMaterial>();
	//wstring path = L"../Resources/Texture/magenta.png";
	//tempMat->Load(path, L"../Resources/Shader/Default.hlsl");

	//shared_ptr<UMaterial> tempSkinnedMat = make_shared<UMaterial>();
	//tempSkinnedMat->Load(path, L"../Resources/Shader/skinningShader.hlsl");
	//tempSkinnedMat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));

	//for (int iFbx = 0; iFbx < m_vFbxList.size(); iFbx++)
	//{
	//	for (int iMesh = 0; iMesh < m_vFbxList[iFbx].m_vMeshList.size(); iMesh++)
	//	{
	//		auto& data = m_vFbxList[iFbx].m_vMeshList[iMesh];
	//		shared_ptr<UMeshComponent> meshComponent;
	//		if (data.m_bSkeleton)
	//		{
	//			meshComponent = make_shared<USkinnedMeshComponent>();
	//			meshComponent->SetName(m_vFbxList[iFbx].m_vMeshList[iMesh].m_szName);
	//			shared_ptr<USkeletalMeshResources> mesh = make_shared<USkeletalMeshResources>();
	//			mesh->SetVertexList(data.m_vVertexList);
	//			mesh->SetInverseBindPose(m_vFbxList[iFbx].m_vInverseBindPose[iMesh]);
	//			dynamic_cast<USkeletalMeshResources*>(mesh.get())->SetIwList(data.m_vIwList);
	//			mesh->Create();
	//			dynamic_cast<USkinnedMeshComponent*>(meshComponent.get())->SetMesh(mesh);
	//			meshComponent->SetMaterial(tempSkinnedMat);
	//			// BONE
	//			map<wstring, BoneNode> bones;
	//			for (auto& data : m_vFbxList[iFbx].m_mSkeletonList)
	//			{
	//				bones.insert(make_pair(data.second.m_szName, data.second));
	//			}
	//			mesh->AddSkeleton(bones);
	//		}
	//		else
	//		{
	//			meshComponent = make_shared<UStaticMeshComponent>();
	//			meshComponent->SetName(m_vFbxList[iFbx].m_vMeshList[iMesh].m_szName);
	//			shared_ptr<UStaticMeshResources> mesh = make_shared<UStaticMeshResources>();
	//			mesh->SetVertexList(data.m_vVertexList);
	//			mesh->Create();
	//			dynamic_cast<UStaticMeshComponent*>(meshComponent.get())->SetMesh(mesh);
	//			meshComponent->SetMaterial(tempMat);
	//		}
	//		m_vMeshList.emplace_back(meshComponent);
	//	}
	//}
	return m_vMeshList;
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
	//vector<shared_ptr<UAnimInstance>> animList;
	//for (int iFbx = 0; iFbx < m_vFbxList.size(); iFbx++)
	//{
	//	// Animation
	//	shared_ptr<UAnimInstance> animInstance = make_shared<UAnimInstance>();
	//	{
	//		animInstance->CreateConstantBuffer();
	//		for (int iAnim = 0; iAnim < m_vFbxList[iFbx].m_iAnimTrackCount; iAnim++)
	//		{
	//			AnimList animTrack;
	//			animTrack.m_szName = m_vFbxList[iFbx].m_vAnimTrackList[iAnim].m_szName;
	//			animTrack.animList = m_vFbxList[iFbx].m_vAnimTrackList[iAnim].m_vAnim;
	//			animInstance->AddTrack(animTrack);
	//		}
	//	}
	//	animList.emplace_back(animInstance);
	//}	
	return m_vAnimInstanceList;
}