#include "pch.h"
#include "TestYR.h"
#include "UStaticMeshComponent.h"
#include "USkinnedComponent.h"
#include "UMaterial.h"
#include "AAsset.h"
#include "APawn.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"

void TestYR::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	m_pCameraActor->Init();


	/*m_pStaticMesh = make_shared<UStaticMeshComponent>();
	m_pStaticMesh->CreateCube();
	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Glow.hlsl");
	m_pStaticMesh->SetMaterial(material);*/

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
	m_pActor2->Init();
}
void TestYR::Update()
{
	m_pCameraActor->Tick();

	m_pActor->Tick();
	m_pActor2->Tick();



}
void TestYR::Render()
{
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pActor2->Render();

}

void TestYR::MakeObjectByAsset(TFbxResource _resource)
{
	m_pActor = make_shared<APawn>();
	shared_ptr<UMaterial> tempMat = make_shared<UMaterial>();
	shared_ptr<UMaterial> tempSkinnedMat = make_shared<UMaterial>();
	// TEXTURE
	for (auto& iter : _resource.m_mTexPathList)
	{
		auto& data = iter.second;

		wstring path = L"../Resources/Texture/";
		wstring texPath = path + data;
		tempMat->Load(texPath, L"../Resources/Shader/Default.hlsl");
		tempMat->SetInputlayout(INPUTLAYOUT->Get(L"Default"));
		tempSkinnedMat->Load(texPath, L"../Resources/Shader/skinningShader.hlsl");
		shared_ptr<Inputlayout> inputlayout = make_shared<Inputlayout>();
		// INPUTLAYOUT 
		{
			INPUTLAYOUT->CreateIW(tempSkinnedMat->GetShader()->m_pCode);
			inputlayout = INPUTLAYOUT->Get(L"IW");
		}
		tempSkinnedMat->SetInputlayout(inputlayout);
	}

 
	/*vector<vector<Matrix>> anim = _resource.m_vAnimTrackList[0].m_vAnim;
	{
		CbAnimData animData;
		for (int i = 0; i < anim.size(); i++)
		{
			animData.boneAnim[i] = _resource.m_vAnimTrackList[0].m_vAnim[i][0];
		}
		D3D11_BUFFER_DESC pDesc;
		ZeroMemory(&pDesc, sizeof(pDesc));
		pDesc.ByteWidth = sizeof(CbAnimData);
		pDesc.Usage = D3D11_USAGE_DEFAULT;
		pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA pInitialData;
		ZeroMemory(&pInitialData, sizeof(pInitialData));
		pInitialData.pSysMem = &animData;
		HRESULT hr = DEVICE->CreateBuffer(&pDesc, &pInitialData, _constantBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			assert(false);
		}
		DC->VSSetConstantBuffers(3, 1, _constantBuffer.GetAddressOf());
	}*/

	// MESH
	shared_ptr<UStaticMeshComponent> mesh = make_shared<UStaticMeshComponent>();
	mesh->m_vVertexList = _resource.m_vMeshList[0].m_vVertexList;
	//mesh->m_vIwList = _resource.m_vMeshList[0].m_vIwList;
	mesh->SetMaterial(tempMat);
	mesh->CreateVertexBuffer();
	//mesh->CreateIndexBuffer();
	//mesh->CreateIwBuffer();

	m_pActor->SetMesh(mesh);

	// MESH
	shared_ptr<USkinnedComponent> skinnedMesh = make_shared<USkinnedComponent>();
	skinnedMesh->m_vVertexList = _resource.m_vMeshList[0].m_vVertexList;
	skinnedMesh->m_vIwList = _resource.m_vMeshList[0].m_vIwList;
	skinnedMesh->SetMaterial(tempSkinnedMat);
	skinnedMesh->CreateVertexBuffer();
	//mesh->CreateIndexBuffer();
	skinnedMesh->CreateIwBuffer();

	m_pActor2->SetMesh(skinnedMesh);
}
