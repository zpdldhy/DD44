#include "pch.h"
#include "TestPlayer.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "ActorLoader.h"
#include "PlayerMoveScript.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"

void TestPlayer::Init()
{
	// 플레이어 세팅
	shared_ptr<PlayerMoveScript> movement = make_shared<PlayerMoveScript>();
	{
		loader = make_shared<ActorLoader>();
		player = loader->LoadOne("../Resources/Asset/crow_final.asset");
		auto cameraComponent = make_shared<UCameraComponent>();
		cameraComponent->SetPosition(Vec3(20.0f, 20.0f, -20.0f));
		player->SetCameraComponent(cameraComponent);
		player->AddScript(movement);

		//map<wstring, shared_ptr<UMeshComponent>> meshList = loader->LoadMesh();
		//dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetAnimInstance(player->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance());
		//dynamic_cast<UStaticMeshComponent*>(meshList[2].get())->SetTargetBoneIndex(43);
		//player->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[2]);
		player->Init();
	}
	// 카메라 세팅
	{
		m_pCameraActor = make_shared<ACameraActor>();
		{
			m_pCameraActor->SetPosition(player->GetPosition());
			m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
		}
		m_pCameraActor->Init();
		CAMERAMANAGER->SetCameraActor(player);
	}

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

		object1->Init();
		object2->Init();
		object3->Init();


		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);
	}
}

void TestPlayer::Update()
{
	m_pCameraActor->Tick();
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Tick();
	}

	player->Tick();
}

void TestPlayer::Render()
{
	player->Render();
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Render();
	}
}
