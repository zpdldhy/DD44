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
#include "ALight.h"
#include "LightManager.h"

void TestYR2::Init()
{
#pragma region 씬 세팅
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);

	SetupLight();
	SetupGizmo();

#pragma endregion

	loader = make_shared<ActorLoader>();
	loader->LoadAllAsset();
	meshResMap = loader->LoadMeshMap();
	animList = loader->LoadAnim();
	texList = loader->LoadTexPath();
	meshLoader.SetMesh(meshResMap);
	meshLoader.SetAnim(loader->LoadAnimMap());


	m_pActor = make_shared<APawn>();
	auto meshComponent = meshLoader.Make("../Resources/Asset/player.mesh.json");
	m_pActor->SetMeshComponent(meshComponent);

	m_pActor->SetPosition(Vec3(10, 10, 10));

	//player->AddScript(movement);
	OBJECT->AddActor(m_pActor);


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
				rootMesh->SetName(data.compName);
				rootMesh->SetMesh(iter->second);
				// ANIM
				auto skinnedRoot = dynamic_pointer_cast<USkinnedMeshComponent>(rootMesh);
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
				rootMesh->SetName(data.compName);
				rootMesh->SetMesh((iter->second));

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
	GUI->SetChildMeshEditorCallback([this](PreMeshData data)
		{
			AddChild(data);
		});

	meshEditor->MoveMesh([this](string compName, Vec3 position)
		{
			auto mesh = m_vActorList[0]->GetMeshComponent();
			auto child = mesh->GetChildByName(to_mw(compName));
			child->SetLocalPosition(position);
		});
	meshEditor->ScaleMesh([this](string compName, Vec3 scale)
		{
			auto mesh = m_vActorList[0]->GetMeshComponent();
			auto child = mesh->GetChildByName(to_mw(compName));
			child->SetLocalScale(scale);
		});
	meshEditor->RotateMesh([this](string compName, Vec3 rot)
		{
			auto mesh = m_vActorList[0]->GetMeshComponent();
			auto child = mesh->GetChildByName(to_mw(compName));
			child->SetLocalRotation(rot);
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
	meshEditor->ChangeParentBone([this](string compName, string bone, bool inverse)
		{
			auto mesh = dynamic_pointer_cast<USkinnedMeshComponent>(m_vActorList[0]->GetMeshComponent());
			auto animInstance = mesh->GetAnimInstance();
			auto child = mesh->GetChildByName(to_mw(compName));
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
	meshEditor->ChangeVisible([this](string compName, bool b)
		{
			auto mesh = m_vActorList[0]->GetMeshComponent();
			auto child = mesh->GetChildByName(to_mw(compName));
			child->SetVisible(b);
		});
	meshEditor->RemoveMesh([this](string _name)
		{
			auto mesh = m_vActorList[0]->GetMeshComponent();
			mesh->RemoveChild(to_mw(_name));
		});
}

void TestYR2::Tick()
{
	if (bSaveMesh)
	{
		AAsset::ExportJsonMesh(m_pActor, name);
		bSaveMesh = false;
	}
}

void TestYR2::Render()
{
}

void TestYR2::Destroy()
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
		child->SetName(data.compName);
		auto iter = meshResMap.find(data.meshName);
		if (iter == meshResMap.end()) { assert(false); }
		child->SetMesh((iter->second));
		// ANIM
		// static 아래에 skinned가 들어갈 수도 있음
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
		if (data.parentCompName != rootMesh->GetName())
		{
			auto parentComp = rootMesh->GetChildByName(data.parentCompName);
			parentComp->AddChild(child);
			child->SetParentTransform(dynamic_pointer_cast<USceneComponent>(parentComp).get());

		}
		else
		{
			rootMesh->AddChild(child);
			child->SetOwner(data.actor);
		}
		child->Init();
	}
	else
	{
		child = make_shared<UStaticMeshComponent>();
		child->SetName(data.compName);
		auto iter = meshResMap.find(data.meshName);
		if (iter == meshResMap.end()) { assert(false); }
		child->SetMesh((iter->second));
		// ANIM
		if (data.bAnimatedStatic)
		{
			auto sChild = dynamic_pointer_cast<UStaticMeshComponent>(child);
			sChild->SetAnimInstance(animInstance);
			if (!data.parentBoneName.empty())
			{
				int parentBone = dynamic_pointer_cast<USkinnedMeshComponent>(rootMesh)->GetMesh()->GetBoneIndex(to_mw(data.parentBoneName));
				Matrix matBone = animInstance->GetBoneAnim(parentBone);
				if (data.bInverseMatBone)
				{
					matBone = matBone.Invert();
				}
				sChild->SetMatBone(matBone);
				sChild->SetAnimInstance(animInstance);
				sChild->SetTargetBoneIndex(parentBone);
			}
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

		if (data.parentCompName != rootMesh->GetName())
		{
			auto parentComp = rootMesh->GetChildByName(data.parentCompName);
			parentComp->AddChild(child);
			child->SetParentTransform(dynamic_pointer_cast<USceneComponent>(parentComp).get());
		}
		else
		{
			rootMesh->AddChild(child);
			child->SetOwner(data.actor);
		}
		child->Init();
	}
}

void TestYR2::SetupLight()
{
	auto m_pSunLight = make_shared<ALight>();
	m_pSunLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
	m_pSunLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
	m_pSunLight->GetLightComponent()->SetAmbientPower(0.3f);
	m_pSunLight->SetPosition(Vec3(0, 100.0f, 0));
	m_pSunLight->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	OBJECT->AddActor(m_pSunLight);

	LIGHTMANAGER->Clear();
	LIGHTMANAGER->RegisterLight(m_pSunLight);
}

void TestYR2::SetupGizmo()
{
	shared_ptr<APawn> object1 = make_shared<APawn>();
	shared_ptr<APawn> object2 = make_shared<APawn>();
	shared_ptr<APawn> object3 = make_shared<APawn>();

	// Material 생성
	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");

	// mesh 생성
	shared_ptr<UStaticMeshComponent> meshComponent1 = UStaticMeshComponent::CreateCube();
	shared_ptr<UStaticMeshComponent> meshComponent2 = UStaticMeshComponent::CreateCube();
	shared_ptr<UStaticMeshComponent> meshComponent3 = UStaticMeshComponent::CreateCube();
	meshComponent1->SetMaterial(material);
	meshComponent2->SetMaterial(material);
	meshComponent3->SetMaterial(material);

	// Object 설정
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
