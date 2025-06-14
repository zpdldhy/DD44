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
#include "Timer.h"
#include "Input.h"
#include "GameCameraMove.h"

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
	auto meshComponent = meshLoader->Make("../Resources/Asset/crow_final.mesh.json");
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
	SetupGameCamera();
	SetupEngineCamera();
#pragma endregion
	
	//{
	//	socket = UStaticMeshComponent::CreateCube();
	//	//shared_ptr<MeshTransform> mt = make_shared<MeshTransform>();
	//	//socket->SetMeshTransform(mt);
	//	sword->AddChild(socket);
	//	socket->SetOwner(player);
	//	socket->Init();
	//	//socket->SetVisible(false);
	//}
	
}

void TestPlayer::Tick()
{
	if (INPUT->GetButtonDown(J))
	{
		m_fHitFlashTimer = 1.f;  // 1초 동안
		m_bIsFlashing = true;
	}

	if (m_bIsFlashing)
	{
		m_fHitFlashTimer -= TIMER->GetDeltaTime();
		if (m_fHitFlashTimer <= 0.0f)
		{
			m_fHitFlashTimer = 0.0f;
			m_bIsFlashing = false;
		}

		// hitFlashAmount는 1 → 0 으로 감소
		float hitFlashAmount = std::min(std::max<float>(m_fHitFlashTimer, 0.0f), 1.0f);
		
		auto root = player->GetMeshComponent();
		ApplyHitFlashToAllMaterials(root, hitFlashAmount);
	}

	if (INPUT->GetButton(O))
	{
		if (m_bEnginCamera)
		{
			m_bEnginCamera = false;
			CAMERA->Set3DCameraActor(m_pGameCameraActor);
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

void TestPlayer::Destroy()
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
	m_pCameraActor = make_shared<ACameraActor>();

	m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
	m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraActor->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);
}


// 빈 함수, 기능 필요하면 넣기용.
void TestPlayer::VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{
	
	}
	
	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		VisitAllMeshMaterials(comp->GetChild(i));
	}
}

void TestPlayer::ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{
		mat->SetHitFlashTime(value); // CB에 g_fHitFlashTime 전달
	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		ApplyHitFlashToAllMaterials(comp->GetChild(i), value);
	}
}

void TestPlayer::SetupGameCamera()
{
	m_pGameCameraActor = make_shared<ACameraActor>();

	//m_pGameCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
	auto script = make_shared<GameCameraMove>(player);
	m_pGameCameraActor->AddScript(script);
	m_pGameCameraActor->m_szName = L"GameCamera";

	CAMERA->Set3DCameraActor(m_pGameCameraActor);
	OBJECT->AddActor(m_pGameCameraActor);

}