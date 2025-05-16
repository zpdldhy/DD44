#include "pch.h"
#include "TestYR.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "Input.h"
#include "UMaterial.h"
#include "UAnimInstance.h"
#include "AnimTrack.h"
#include "ImGuiCore.h"
#include "UIManager.h"
#include "ObjectManager.h"

void TestYR::Init()
{
#pragma region Gizmo 세팅
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
#pragma endregion

#pragma region 카메라 세팅
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);
#pragma endregion

	//loader.ConvertFbxToAsset();

	m_vActorList = loader.LoadAllActor();
	for (int i = 0; i < m_vActorList.size(); i++)
	{
		m_vActorList[i]->SetPosition(Vec3(10.0f * i , 0, 0));
		OBJECT->AddActor(m_vActorList[i]);
	}

#pragma region ActorTest
	{
		m_pActor = loader.LoadOneActor("../Resources/Asset/crow_final.asset");
		tempmeshList = loader.LoadMesh();
		targetObj = m_pActor;
		auto animInstance = m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();

#pragma region weapon
		{
			auto weapon = dynamic_cast<UStaticMeshComponent*>(tempmeshList[2].get());

			Matrix matBone = animInstance->GetBoneAnim(43);
			weapon->SetMatBone(matBone);

			weapon->SetAnimInstance(animInstance);
			weapon->SetTargetBoneIndex(43);
			weapon->SetLocalPosition(Vec3(-1.15f, 0.2f, -0.8f));
			//weapon->SetLocalRotation(Vec3(0.0f, 0.0f, 10.0f));
			m_pActor->GetMeshComponent<USkinnedMeshComponent>()->AddChild(tempmeshList[2]);
		}
#pragma endregion

#pragma region eye
		{
			auto eye = dynamic_cast<UStaticMeshComponent*>(tempmeshList[1].get());

			Matrix matBone = animInstance->GetBoneAnim(57);
			eye->SetMatBone(matBone);

			eye->SetAnimInstance(animInstance);
			eye->SetTargetBoneIndex(57);
			// Idle
			eye->SetLocalPosition(Vec3(-0.45f, 0.36f, -0.25f));
			m_pActor->GetMeshComponent<USkinnedMeshComponent>()->AddChild(tempmeshList[1]);
		}
#pragma endregion

	}
#pragma endregion

	//OBJECTMANAGER->AddActor(m_pActor);
}
void TestYR::Update()
{
	if (INPUT->GetButton(LCLICK))
	{
		int targetIndex = m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance()->GetAnimIndex(L"Slash_Light_R_new");
		m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance()->PlayOnce(targetIndex);
	}
}
void TestYR::Render()
{
}


