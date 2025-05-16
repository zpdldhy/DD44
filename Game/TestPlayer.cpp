#include "pch.h"
#include "TestPlayer.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "ActorLoader.h"
#include "PlayerMoveScript.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "UIManager.h"
#include "ObjectManager.h"

void TestPlayer::Init()
{
	#pragma region PLAYER
	// 플레이어 세팅
	shared_ptr<PlayerMoveScript> movement = make_shared<PlayerMoveScript>();
	{
		loader = make_shared<ActorLoader>();
		player = loader->LoadOneActor("../Resources/Asset/crow_final.asset");
		meshList = loader->LoadMesh();
		auto cameraComponent = make_shared<UCameraComponent>();
		cameraComponent->SetLocalPosition(Vec3(20.0f, 20.0f, -20.0f));
		player->SetCameraComponent(cameraComponent);
		player->AddScript(movement);

		{
			auto animInstance = player->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();

			{
				auto weapon = dynamic_cast<UStaticMeshComponent*>(meshList[2].get());

				Matrix matBone = animInstance->GetBoneAnim(43);
				weapon->SetMatBone(matBone);

				weapon->SetAnimInstance(animInstance);
				weapon->SetTargetBoneIndex(43);
				weapon->SetLocalPosition(Vec3(-1.15f, 0.2f, -0.8f));
				player->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[2]);
			}

			{
				auto eye = dynamic_cast<UStaticMeshComponent*>(meshList[1].get());

				Matrix matBone = animInstance->GetBoneAnim(57);
				eye->SetMatBone(matBone);

				eye->SetAnimInstance(animInstance);
				eye->SetTargetBoneIndex(57);
				// Idle
				eye->SetLocalPosition(Vec3(-0.45f, 0.36f, -0.25f));
				player->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[1]);
			}
		}
		OBJECTMANAGER->AddActor(player);
	}
	#pragma endregion

	#pragma region CAMERA
	// 카메라 세팅
	{
		m_pCameraActor = make_shared<ACameraActor>();
		{
			m_pCameraActor->SetPosition(player->GetPosition());
			m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
		}
		CAMERAMANAGER->Set3DCameraActor(player);
		OBJECTMANAGER->AddActor(m_pCameraActor);
	}
	#pragma endregion

	#pragma region GIZMO

	//Gizmo 세팅
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

		object1->SetScale(Vec3(1000.0f, 0.03f, 0.03f));
		object2->SetScale(Vec3(0.03f, 1000.0f, 0.03f));
		object3->SetScale(Vec3(0.03f, 0.03f, 1000.0f));

		OBJECTMANAGER->AddActor(object1);
		OBJECTMANAGER->AddActor(object2);
		OBJECTMANAGER->AddActor(object3);


		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);
	}
#pragma endregion

	#pragma region TEMP_PROJECTILE
	projectile = make_shared<APawn>();
	
	shared_ptr<UStaticMeshComponent> meshComponent1 = UStaticMeshComponent::CreateCube();
	shared_ptr<UMaterial> mat = make_shared<UMaterial>();
	mat->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
	meshComponent1->SetMaterial(mat);
	meshComponent1->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));
	projectile->SetMeshComponent(meshComponent1);
	OBJECTMANAGER->AddActor(projectile);
	#pragma endregion
}

void TestPlayer::Update()
{
	
}

void TestPlayer::Render()
{
}
