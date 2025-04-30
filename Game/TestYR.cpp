#include "pch.h"
#include "TestYR.h"
#include "UStaticMeshComponent.h"
#include "USkinnedComponent.h"
#include "UMaterial.h"
#include "AAsset.h"
#include "APawn.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "UAnimation.h"

void TestYR::Init()
{

	//Gizmo 세팅
	{
		shared_ptr<APawn> object1 = make_shared<APawn>();
		shared_ptr<APawn> object2 = make_shared<APawn>();
		shared_ptr<APawn> object3 = make_shared<APawn>();

		// Material 생성
		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");

		// mesh 생성
		shared_ptr<UStaticMeshComponent> mesh = make_shared<UStaticMeshComponent>();
		mesh->CreateCube();
		mesh->SetMaterial(material);

		// Object 설정
		object1->SetMesh(mesh);
		object2->SetMesh(mesh);
		object3->SetMesh(mesh);

		object1->SetScale(Vec3(1000.0f, 0.03f, 0.03f));
		object2->SetScale(Vec3(0.03f, 1000.0f, 0.03f));
		object3->SetScale(Vec3(0.03f, 0.03f, 1000.0f));
		
		object1->Init();
		object2->Init();
		object3->Init();
		
		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);
	}

	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	m_pCameraActor->Init();

	// FBX 로드 및 파일 출력
	{
		//string path = "../Resources/Fbx/DeathDoor/*.fbx";
		//HANDLE hFind;
		//WIN32_FIND_DATAA data;
		//vector<string> fileList;
		//if ((hFind = FindFirstFileA(path.c_str(), &data)) != INVALID_HANDLE_VALUE)
		//{
		//	do
		//	{
		//		string directory = "../Resources/Fbx/DeathDoor/";
		//		directory += string(data.cFileName);
		//		fileList.emplace_back(directory);
		//	} while (FindNextFileA(hFind, &data) != 0);
		//	FindClose(hFind);
		//}

		//for (int iPath = 0; iPath < fileList.size(); iPath++)
		//{
		//	TFbxResource ret = m_FbxImporter.Load(fileList[iPath]);
		//	//fbxList.emplace_back(ret);
		//	AAsset::Export(ret, fileList[iPath]);
		//}
	}

	vector<TFbxResource> fbxList;
	// 파일 읽기
	{
		string path = "../Resources/Asset/*.asset";
		HANDLE hFind;
		WIN32_FIND_DATAA data;
		vector<string> fileList;
		if ((hFind = FindFirstFileA(path.c_str(), &data)) != INVALID_HANDLE_VALUE) 
		{
			do
			{
				string directory = "../Resources/Asset/";
				directory += string(data.cFileName);
				fileList.emplace_back(directory);
			} while (FindNextFileA(hFind, &data) != 0);
			FindClose(hFind);
		}

		for (int iPath = 0; iPath < fileList.size(); iPath++)
		{
			fbxList.emplace_back(AAsset::Load(fileList[iPath].c_str()));
		}
	}
	MakeObjectByAsset(fbxList[0]);
	m_pActor->Init();
}
void TestYR::Update()
{
	m_pCameraActor->Tick();
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Tick();
	}
	m_pActor->Tick();
}
void TestYR::Render()
{
	m_pCameraActor->Render();
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Render();
	}
	m_pActor->Render();
}

void TestYR::MakeObjectByAsset(TFbxResource _resource)
{
	m_pActor = make_shared<APawn>();

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
	vector<shared_ptr<UAnimation>> meshAnim; 
	{
		shared_ptr<UAnimation> animTrack;
		for (int iMesh = 0; iMesh < _resource.m_iMeshCount; iMesh++)
		{
			animTrack = make_shared<UAnimation>();
			animTrack->animTrackList.resize(_resource.m_iAnimTrackCount);
			for (int iAnim = 0; iAnim < _resource.m_iAnimTrackCount; iAnim++)
			{
				animTrack->animTrackList[iAnim].animList = _resource.m_vAnimTrackList[iAnim].m_vAnim;
				animTrack->inversedBone = _resource.m_vInverseBindPose[iMesh];
			}
			animTrack->CreateConstantBuffer();
			meshAnim.emplace_back(animTrack);
		}
		
	}

	////MESH
	shared_ptr<USkinnedComponent> skinnedMesh = make_shared<USkinnedComponent>();
	skinnedMesh->m_vVertexList = _resource.m_vMeshList[0].m_vVertexList;
	skinnedMesh->m_vIwList = _resource.m_vMeshList[0].m_vIwList;
	skinnedMesh->SetMaterial(tempSkinnedMat);
	skinnedMesh->CreateVertexBuffer();
	skinnedMesh->CreateIwBuffer();
	skinnedMesh->m_pAnim = meshAnim[0];

	////MESH
	shared_ptr<USkinnedComponent> tailMesh = make_shared<USkinnedComponent>();
	tailMesh->m_vVertexList = _resource.m_vMeshList[5].m_vVertexList;
	tailMesh->m_vIwList = _resource.m_vMeshList[5].m_vIwList;
	tailMesh->SetMaterial(tempSkinnedMat);
	tailMesh->CreateVertexBuffer();
	tailMesh->CreateIwBuffer();
	tailMesh->m_pAnim = meshAnim[5];
	
	shared_ptr<UMaterial> weaponMat = make_shared<UMaterial>();
	weaponMat->Load(L"../Resources/Texture/Crow_DIFF.png", L"../Resources/Shader/Default.hlsl");

	skinnedMesh->AddChild(tailMesh);

	m_pActor->SetMesh(skinnedMesh);
}
