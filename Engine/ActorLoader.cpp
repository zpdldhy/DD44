#include "pch.h"
#include "ActorLoader.h"
#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "UMaterial.h"
#include "AAsset.h"
#include "APawn.h"
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

vector<shared_ptr<APawn>> ActorLoader::Load()
{
	vector<shared_ptr<APawn>> actorList;
	// 파일 이름 
	vector<string> fileNames = GetFileNames("../Resources/Asset/*.asset");

	// TFbxResource 파싱
	vector<TFbxResource> fbxList;
	for (int iPath = 0; iPath < fileNames.size(); iPath++)
	{
		fbxList.emplace_back(AAsset::Load(fileNames[iPath].c_str()));
	}

	// actor 변환
	for (int iFbx = 0; iFbx < fbxList.size(); iFbx++)
	{
		auto& _resource = fbxList[iFbx];
		
		shared_ptr<APawn> actor = make_shared<APawn>();
		shared_ptr<UMaterial> tempSkinnedMat = make_shared<UMaterial>();
		// TEXTURE
		for (auto& iter : _resource.m_mTexPathList)
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
		}

		// Animation
		shared_ptr<UAnimInstance> animInstance = make_shared<UAnimInstance>();
		{
			animInstance->SetInverseBone(_resource.m_vInverseBindPose);
			animInstance->CreateConstantBuffer();
			for (int iAnim = 0; iAnim < _resource.m_iAnimTrackCount; iAnim++)
			{
				AnimList animTrack;
				animTrack.m_szName = _resource.m_vAnimTrackList[iAnim].m_szName;
				animTrack.animList= _resource.m_vAnimTrackList[iAnim].m_vAnim;
				animInstance->AddTrack(animTrack);
			}
		}

		////MESH
		// TEMP
		shared_ptr<USkinnedMeshComponent> rootMesh = make_shared<USkinnedMeshComponent>();
		shared_ptr<USkeletalMeshResources> mesh = make_shared< USkeletalMeshResources>();
		mesh->SetVertexList(_resource.m_vMeshList[0].m_vVertexList);
		mesh->SetIwList(_resource.m_vMeshList[0].m_vIwList);
		mesh->Create();
		rootMesh->SetMesh(mesh);
		rootMesh->SetMaterial(tempSkinnedMat);
		rootMesh->SetBaseAnim(animInstance);
		shared_ptr<AnimTrack> animTrack = make_shared< AnimTrack>();
		animTrack->SetBase(animInstance, 0);
		rootMesh->SetMeshAnim(animTrack);

		//
		for (int iMesh = 1; iMesh < _resource.m_vMeshList.size(); iMesh++)
		{
			if (_resource.m_vMeshList[iMesh].m_bSkeleton)
			{
				shared_ptr<USkinnedMeshComponent> meshComponent = make_shared<USkinnedMeshComponent>();
				shared_ptr<USkeletalMeshResources> mesh = make_shared< USkeletalMeshResources>();
				mesh->SetVertexList(_resource.m_vMeshList[iMesh].m_vVertexList);
				mesh->SetIwList(_resource.m_vMeshList[iMesh].m_vIwList);
				mesh->Create();
				meshComponent->SetMesh(mesh);
				meshComponent->SetMaterial(tempSkinnedMat);
				shared_ptr<AnimTrack> animTrack = make_shared< AnimTrack>();
				animTrack->SetBase(animInstance, iMesh);
				meshComponent->SetMeshAnim(animTrack);
				rootMesh->AddChild(meshComponent);
			}

		}

		actor->SetMeshComponent(rootMesh);
		actorList.emplace_back(actor);
	}

	return actorList;
}