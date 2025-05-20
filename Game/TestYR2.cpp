#include "pch.h"
#include "TestYR2.h"
#include "ActorLoader.h"
#include "ObjectLoader.h"
#include "UMeshComponent.h"
#include "UAnimInstance.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "ObjectManager.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "Inputlayout.h"
#include "AnimTrack.h"
#include "APawn.h"
#include "AAsset.h"
#include "Input.h"

void TestYR2::Init()
{
#pragma region Camera
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);
#pragma endregion
#pragma region Gizmo 技泼
	{
		shared_ptr<APawn> object1 = make_shared<APawn>();
		shared_ptr<APawn> object2 = make_shared<APawn>();
		shared_ptr<APawn> object3 = make_shared<APawn>();

		// Material 积己
		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");

		// mesh 积己
		shared_ptr<UStaticMeshComponent> meshComponent1 = UStaticMeshComponent::CreateCube();
		shared_ptr<UStaticMeshComponent> meshComponent2 = UStaticMeshComponent::CreateCube();
		shared_ptr<UStaticMeshComponent> meshComponent3 = UStaticMeshComponent::CreateCube();
		meshComponent1->SetMaterial(material);
		meshComponent2->SetMaterial(material);
		meshComponent3->SetMaterial(material);

		// Object 汲沥
		object1->SetMeshComponent(meshComponent1);
		object2->SetMeshComponent(meshComponent2);
		object3->SetMeshComponent(meshComponent3);

		object1->SetScale(Vec3(10000.0f, 0.03f, 0.03f));
		object2->SetScale(Vec3(0.03f, 10000.0f, 0.03f));
		object3->SetScale(Vec3(0.03f, 0.03f, 10000.0f));

		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);

		OBJECT->AddActorList(gizmo);
	}
#pragma endregion


	//loader = make_shared<ActorLoader>();
	//loader->ConvertFbxToAsset();
	//string path = "../Resources/Asset/_E_KNIGHT.asset";
	//name = to_wm(SplitName(to_mw(path)));
	//m_pActor = loader->LoadRedeemer(path);

	//OBJECT->AddActor(m_pActor);


	//loader = make_shared<ActorLoader>();
	////name = "temp";
	//loader->LoadAllAsset();
	//meshResMap = loader->LoadMeshMap();
	//animList = loader->LoadAnim();
	//texList = loader->LoadTexPath();

	objLoader = make_shared< ObjectLoader>();
	objLoader->ConvertObjToAsset("../Resources/Obj/wall/*.obj");
	//objLoader->Load();
	//objLoader->LoadAsFR("../Resources/Obj/AltarSmallStone3.obj");
	//map<wstring, shared_ptr<UMeshResources>> tempMap = objLoader->LoadMeshMap();
	loader = make_shared<ActorLoader>();
	name = "temp";
	loader->LoadAllAsset();
	meshResMap = loader->LoadMeshMap();
	
	/*for (auto& data : tempMap)
	{
		meshResMap.insert(data);
	}*/

	{

	}

	auto meshEditor = GUI->GetMeshEditorUI();
	//meshEditor->SetActor(m_pActor);
	meshEditor->SetMeshList(meshResMap);
	meshEditor->SetAnimList(animList);
	meshEditor->SetTexList(texList);

	GUI->SetBaseMeshEditorCallback([this](PreMeshData data, shared_ptr<APawn>& _pActor)
		{
			_pActor = make_shared<APawn>();
			m_pActor = _pActor;
			// MESH
			////Base
			auto iter = meshResMap.find(data.meshName);
			if (iter == meshResMap.end()) { assert(false); }
			shared_ptr<UMeshComponent> rootMesh;

			if (iter->second->GetType() == (int)MeshType::M_SKINNED)
			{
				rootMesh = make_shared <USkinnedMeshComponent>();
				auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(rootMesh);
				skinnedRoot->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(iter->second));
				// ANIM
				auto animInstance = animList[data.animIndex];
				skinnedRoot->SetBaseAnim(animInstance);
				shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
				animTrack->SetBase(animList[data.animIndex]);
				skinnedRoot->SetMeshAnim(animTrack);

				// MATERIAL
				shared_ptr<UMaterial> mat = make_shared<UMaterial>();
				wstring texPath = L"../Resources/Texture/";
				if (!data.texPath.empty())
				{
					texPath += to_mw(data.texPath);
				}
				else
				{
					texPath += texList[data.texIndex];
				}
				wstring shaderPath = L"../Resources/Shader/";
				if (!data.shaderPath.empty())
				{
					shaderPath += to_mw(data.shaderPath);
				}
				else
				{
					shaderPath += L"skinningShader.hlsl";
				}
				mat->Load(texPath, shaderPath);
				mat->SetSpecularParams(data.specular, data.shininess);
				mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
				rootMesh->SetMaterial(mat);

				m_pActor->SetMeshComponent(rootMesh);

				// ROOTMOTION
				if (strcmp(data.rootBoneName.c_str(), ""))
				{
					int rootBone = skinnedRoot->GetMesh()->GetBoneIndex(to_mw(data.rootBoneName));

					animList[data.animIndex]->CheckInPlace(true);
					animList[data.animIndex]->SetRootIndex(rootBone);
				}

			}
			else
			{
				rootMesh = make_shared <UStaticMeshComponent>();
				auto staticRoot = dynamic_pointer_cast<UStaticMeshComponent>(rootMesh);

				staticRoot->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(iter->second));

				// MATERIAL
				shared_ptr<UMaterial> mat = make_shared<UMaterial>();
				wstring texPath = L"../Resources/Texture/";
				if (!data.texPath.empty())
				{
					texPath += to_mw(data.texPath);
				}
				else
				{
					texPath += texList[data.texIndex];
				}
				wstring shaderPath = L"../Resources/Shader/";
				if (!data.shaderPath.empty())
				{
					shaderPath += to_mw(data.shaderPath);
				}
				else
				{
					shaderPath += L"Default.hlsl";
				}
				mat->Load(texPath, shaderPath);
				//mat->SetSpecularParams(data.specular, data.shininess);
				rootMesh->SetMaterial(mat);

				m_pActor->SetMeshComponent(rootMesh);

			}

			m_vActorList.emplace_back(m_pActor);
			OBJECT->AddActor(m_pActor);
		});
	GUI->SetChildMeshEditorCallback([this](PreMeshData data, shared_ptr<APawn>& _pActor)
		{
			AddChild(data);
		});

	meshEditor->MoveMesh([this](int meshIndex, Vec3 position)
		{
			auto mesh = m_vActorList[0]->GetMeshComponent();
			auto child = mesh->GetChild(meshIndex);
			child->SetLocalPosition(position);
		});
	meshEditor->StopAnim([this](bool stop)
		{
			auto animInstance = m_vActorList[0]->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
			animInstance->m_bPlay = !stop;
		});
	meshEditor->ChangeAhim([this](int currentAnim)
		{
			auto animInstance = m_vActorList[0]->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
			animInstance->SetCurrentAnimTrack(currentAnim);
		});
	meshEditor->ChangeParentBone([this](int childIndex, string bone, bool inverse)
		{
			auto mesh = dynamic_pointer_cast<USkinnedMeshComponent>(m_vActorList[0]->GetMeshComponent());
			auto animInstance = mesh->GetAnimInstance();
			auto child = mesh->GetChild(childIndex);
			int parentBone = mesh->GetMesh()->GetBoneIndex(to_mw(bone));
			Matrix matBone = animInstance->GetBoneAnim(parentBone);
			if (inverse)
			{
				matBone = matBone.Invert();
			}
			dynamic_pointer_cast<UStaticMeshComponent>(child)->SetMatBone(matBone);
		});
	meshEditor->SaveMesh([this](int i, bool b, string _name)
		{
			bSaveMesh = true;
			name = _name;
		});



}

void TestYR2::Update()
{
	if (bSaveMesh)
	{
		AAsset::ExportJsonMesh(m_pActor, name);
		bSaveMesh = false;
	}

	if (INPUT->GetButton(C))
	{
		//auto anim = m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		//if (++animIndex >= anim->GetAnimTrackList().size())
		//{
		//	animIndex = 0;
		//}
		//anim->SetCurrentAnimTrack(animIndex);
	}
}

void TestYR2::Render()
{
}

void TestYR2::AddChild(PreMeshData data)
{
	shared_ptr<UMeshComponent> child;
	auto rootMesh = data.actor->GetMeshComponent<USkinnedMeshComponent>();
	auto animInstance = rootMesh->GetAnimInstance();

	wstring texPath = L"../Resources/Texture/";
	texPath += to_mw(data.texPath);

	if (data.bSkeletal)
	{
		child = make_shared<USkinnedMeshComponent>();
		auto iter = meshResMap.find(data.meshName);
		if (iter == meshResMap.end()) { assert(false); }
		dynamic_pointer_cast<USkinnedMeshComponent>(child)->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(iter->second));
		// ANIM
		shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
		animTrack->SetBase(animInstance);
		dynamic_pointer_cast<USkinnedMeshComponent>(child)->SetMeshAnim(animTrack);
		// MATERIAL
		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		wstring shaderPath = to_mw(data.shaderPath);
		if (shaderPath.empty())
		{
			shaderPath = L"../Resources/Shader/skinningShader.hlsl";
		}
		mat->Load(texPath, shaderPath);
		mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
		mat->SetSpecularParams(data.specular, data.shininess);
		child->SetMaterial(mat);
		// ROOT
		rootMesh->AddChild(child);
		child->SetOwner(data.actor);
		child->Init();
	}
	else
	{
		child = make_shared<UStaticMeshComponent>();
		auto sChild = dynamic_pointer_cast<UStaticMeshComponent>(child);
		auto iter = meshResMap.find(data.meshName);
		if (iter == meshResMap.end()) { assert(false); }
		sChild->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(iter->second));
		// ANIM
		sChild->SetAnimInstance(animInstance);
		if (!data.parentBoneName.empty())
		{
			int parentBone = rootMesh->GetMesh()->GetBoneIndex(to_mw(data.parentBoneName));
			Matrix matBone = animInstance->GetBoneAnim(parentBone);
			if (data.bInverseMatBone)
			{
				matBone = matBone.Invert();
			}
			sChild->SetMatBone(matBone);
			sChild->SetAnimInstance(animInstance);
			sChild->SetTargetBoneIndex(parentBone);
		}

		// MATERIAL
		shared_ptr<UMaterial> mat = make_shared<UMaterial>();
		wstring shaderPath = to_mw(data.shaderPath);
		if (shaderPath.empty())
		{
			shaderPath = L"../Resources/Shader/Default.hlsl";
		}
		mat->Load(texPath, shaderPath);
		mat->SetSpecularParams(data.specular, data.shininess);
		child->SetMaterial(mat);

		rootMesh->AddChild(sChild);

		sChild->SetOwner(data.actor);
		sChild->Init();
	}
}