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
	//loader->ConvertFbxToAsset();
	loader->LoadOne("../Resources/Asset/_E_BAT_Black Variant.asset");
	meshResList = loader->LoadMeshResources();
	animList = loader->LoadAnim();
	texList = loader->LoadTexPath();


	auto meshEditor = GUI->GetMeshEditorUI();
	meshEditor->SetMeshList(meshResList);
	meshEditor->SetAnimList(animList);
	meshEditor->SetTexList(texList);

	GUI->SetBaseMeshEditorCallback([this](int _mesh, int _anim, int _tex, shared_ptr<APawn>& _pActor)
		{
			_pActor = make_shared<APawn>();
			// MESH
			shared_ptr<USkinnedMeshComponent> rootMesh = make_shared<USkinnedMeshComponent>();
			rootMesh->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(meshResList[_mesh]));
			_pActor->SetMeshComponent(rootMesh);
			// ANIM
			rootMesh->SetBaseAnim(animList[_anim]);
			shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
			animTrack->SetBase(animList[_anim]);
			rootMesh->SetMeshAnim(animTrack);
			// MATERIAL
			shared_ptr<UMaterial> mat = make_shared<UMaterial>();
			wstring texPath = L"../Resources/Texture/";
			texPath += texList[_tex];
			mat->Load(texPath, L"../Resources/Shader/skinningShader.hlsl");
			shared_ptr<Shader> shader = make_shared<Shader>();
			shader = SHADER->Get(L"../Resources/Shader/skinningShader.hlsl");
			INPUTLAYOUT->CreateIW(shader->m_pCode);
			mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
			rootMesh->SetMaterial(mat);

			m_vActorList.emplace_back(_pActor);
			OBJECTMANAGER->AddActor(_pActor);
		});
	GUI->SetChildMeshEditorCallback([this](ChildMeshData data)
		{
			shared_ptr<UMeshComponent> child;
			auto rootMesh = data.rootMesh->GetMeshComponent<USkinnedMeshComponent>();
			auto animInstance = rootMesh->GetAnimInstance();
			if (data.bSkeletal)
			{
				child = make_shared<USkinnedMeshComponent>();
				dynamic_pointer_cast<USkinnedMeshComponent>(child)->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(meshResList[data.meshIndex]));
				// ANIM
				shared_ptr<AnimTrack> animTrack = make_shared<AnimTrack>();
				animTrack->SetBase(animInstance);
				dynamic_pointer_cast<USkinnedMeshComponent>(child)->SetMeshAnim(animTrack);
				// MATERIAL
				shared_ptr<UMaterial> mat = make_shared<UMaterial>();
				mat->Load(to_mw(data.texPath), L"../Resources/Shader/skinningShader.hlsl");
				mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
				child->SetMaterial(mat);
				// ROOT
				rootMesh->AddChild(child);
				child->SetOwner(data.rootMesh);
				child->Init();
			}
			else
			{	
				child = make_shared<UStaticMeshComponent>();
				auto sChild = dynamic_pointer_cast<UStaticMeshComponent>(child);
				sChild->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(meshResList[data.meshIndex]));
				// ANIM
				sChild->SetAnimInstance(animInstance);
				int parentBone = rootMesh->GetMesh()->GetBoneIndex(to_mw(data.parentBoneName));
				Matrix matBone = animInstance->GetBoneAnim(parentBone);
				sChild->SetMatBone(matBone);
				// MATERIAL
				shared_ptr<UMaterial> mat = make_shared<UMaterial>();
				mat->Load(to_mw(data.texPath), L"../Resources/Shader/Default.hlsl");
				child->SetMaterial(mat);
				sChild->SetAnimInstance(animInstance);
				sChild->SetTargetBoneIndex(parentBone);

				rootMesh->AddChild(sChild);

				sChild->SetOwner(data.rootMesh);
				sChild->Init();

			}
		});
	
	meshEditor->MoveMesh([this](int meshIndex, Vec3 position)
	{
		auto mesh = m_vActorList[0]->GetMeshComponent();
		auto child = mesh->GetChild(meshIndex);
		child->SetLocalPosition(position);
	});
}

void TestYR2::Update()
{
}

void TestYR2::Render()
{
}
