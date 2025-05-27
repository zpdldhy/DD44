#include "pch.h"
#include "TestPlayer.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "ActorLoader.h"
#include "MeshLoader.h"
#include "PlayerMoveScript.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "UIManager.h"
#include "ObjectManager.h"
#include "LightManager.h"
#include "ALight.h"
#include "APawn.h"
#include "UMaterial.h"
#include "MeshTransform.h"
#include "Input.h"

void TestPlayer::Init()
{
#pragma region SetupScene

	SetupSunLight();
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

		OBJECT->AddActor(object1);
		OBJECT->AddActor(object2);
		OBJECT->AddActor(object3);


		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);
	}
#pragma endregion
#pragma region LOAD_ASSET

	ACTORLOADER->LoadAllAsset();
	meshLoader = make_shared<MeshLoader>();
	meshLoader->SetMesh(ACTORLOADER->LoadMeshMap());
	meshLoader->SetAnim(ACTORLOADER->LoadAnimMap());
#pragma endregion
#pragma region PLAYER
	// 플레이어 세팅
	shared_ptr<PlayerMoveScript> movement = make_shared<PlayerMoveScript>();
	player = make_shared<APawn>();
	auto meshComponent = meshLoader->Make("../Resources/Asset/crow_final6.mesh.json");
	player->SetMeshComponent(meshComponent);

	player->SetPosition(Vec3(10, 10, 10));
	
	auto cameraComponent = make_shared<UCameraComponent>();
	cameraComponent->SetLocalPosition(Vec3(20.0f, 20.0f, -20.0f));
	player->SetCameraComponent(cameraComponent);
	player->AddScript(movement);
	OBJECT->AddActor(player);

	sword = player->GetMeshComponent()->GetChild(3);
#pragma endregion
#pragma region CAMERA
	// 카메라 세팅
	{
		m_pCameraActor = make_shared<ACameraActor>();
		{
			m_pCameraActor->SetPosition(player->GetPosition());
			m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
		}
		CAMERA->Set3DCameraActor(player);
		OBJECT->AddActor(m_pCameraActor);
	}
#pragma endregion
	
	{
		socket = UStaticMeshComponent::CreateCube();
		//shared_ptr<MeshTransform> mt = make_shared<MeshTransform>();
		//socket->SetMeshTransform(mt);
		sword->AddChild(socket);
		socket->SetOwner(player);
		socket->Init();
		//socket->SetVisible(false);
	}
	
}

void TestPlayer::Tick()
{
	//socket->SetLocalPosition(sword->GetAnimWorld());

	if (INPUT->GetButton(O))
	{
		if (m_bEnginCamera)
		{
			m_bEnginCamera = false;
			CAMERA->Set3DCameraActor(player);
		}
		else
		{
			m_bEnginCamera = true;
			CAMERA->Set3DCameraActor(m_pCameraActor);
		}
	}
}

void TestPlayer::Render()
{
}

void TestPlayer::SetupSunLight()
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

void TestPlayer::SetupEngineCamera()
{
	m_pCameraEngine = make_shared<ACameraActor>();

	m_pCameraEngine->SetPosition({ 0.0f, 10.0f, 0.0f });
	m_pCameraEngine->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraEngine->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(m_pCameraEngine);
	OBJECT->AddActor(m_pCameraEngine);
}