#include "pch.h"
#include "TestYR2.h"
#include "ActorLoader.h"
#include "UMeshComponent.h"
#include "UAnimInstance.h"
#include "ImGuiCore.h"
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

void TestYR2::Init()
{
#pragma region Camera
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	CAMERAMANAGER->Set3DCameraActor(m_pCameraActor);
	OBJECTMANAGER->AddActor(m_pCameraActor);
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

		OBJECTMANAGER->AddActorList(gizmo);
	}
#pragma endregion
	loader = make_shared<ActorLoader>();
	loader->LoadOne("../Resources/Asset/crow_final.asset");
	meshList = loader->LoadMesh();
	animList = loader->LoadAnim();
	texList = loader->LoadTexPath();


	auto meshEditor = GUI->GetMeshEditorUI();
	meshEditor->SetMeshList(meshList);
	meshEditor->SetAnimList(animList);
	meshEditor->SetTexList(texList);

	GUI->SetBaseMeshEditorCallback([this](int _mesh, int _anim, int _tex, shared_ptr<APawn>& _pActor)
		{
			_pActor = make_shared<APawn>();
			// MESH
			_pActor->SetMeshComponent(meshList[_mesh]);
			// ANIM
			dynamic_cast<USkinnedMeshComponent*>(meshList[_mesh].get())->SetBaseAnim(animList[_anim]);
			shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
			animTrack->SetBase(animList[_anim]);
			dynamic_cast<USkinnedMeshComponent*>(meshList[_mesh].get())->SetMeshAnim(animTrack);
			// MATERIAL
			shared_ptr<UMaterial> mat = make_shared<UMaterial>();
			wstring texPath = L"../Resources/Texture/";
			texPath += texList[_tex];
			mat->Load(texPath, L"../Resources/Shader/skinningShader.hlsl");
			shared_ptr<Shader> shader = make_shared<Shader>();
			shader = SHADER->Get(L"../Resources/Shader/skinningShader.hlsl");
			INPUTLAYOUT->CreateIW(shader->m_pCode);
			mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
			meshList[_mesh]->SetMaterial(mat);

			m_vActorList.emplace_back(_pActor);
			OBJECTMANAGER->AddActor(_pActor);
		});
	GUI->SetChildMeshEditorCallback([this](ChildMeshData data)
		{
			auto mesh = meshList[data.meshIndex];
			auto rootMesh = data.rootMesh->GetMeshComponent<USkinnedMeshComponent>();
			auto animInstance = rootMesh->GetAnimInstance();
			if (data.bSkeletal)
			{
				shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
				animTrack->SetBase(animInstance);
				dynamic_pointer_cast<USkinnedMeshComponent>(mesh)->SetMeshAnim(animTrack);
				rootMesh->AddChild(mesh);
				mesh->SetOwner(data.rootMesh);
				mesh->Init();
			}
			else
			{
				auto sChild = dynamic_pointer_cast<UStaticMeshComponent>(mesh);
				sChild->SetAnimInstance(animInstance);
				int parentBone = rootMesh->GetMesh()->GetBoneIndex(to_mw(data.parentBoneName));
				Matrix matBone = animInstance->GetBoneAnim(parentBone);
				sChild->SetMatBone(matBone);

				sChild->SetAnimInstance(animInstance);
				sChild->SetTargetBoneIndex(parentBone);
				//weapon->SetLocalPosition(Vec3(-1.15f, 0.2f, -0.8f));
				//weapon->SetLocalRotation(Vec3(0.0f, 0.0f, 10.0f));
				rootMesh->AddChild(mesh);

				sChild->SetOwner(data.rootMesh);
				sChild->Init();

			}
		});
	//meshEditor->MoveMesh([this](int meshIndex, Vec3 position)
	//{
	//	auto mesh = m_vActorList[0]->GetMeshComponent();
	//});
}

void TestYR2::Update()
{
}

void TestYR2::Render()
{
}
