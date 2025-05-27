#include "pch.h"
#include "TestYR.h"
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
#include "AnimTrack.h"
#include "AAsset.h"

void TestYR::Init()
{
#pragma region SetupScene
	SetupEngineCamera();
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

//	ACTORLOADER->LoadAllAsset();
//	meshLoader = make_shared<MeshLoader>();
//	meshLoader->SetMesh(ACTORLOADER->LoadMeshMap());
//	meshLoader->SetAnim(ACTORLOADER->LoadAnimMap());
//#pragma endregion
//#pragma region PLAYER 
//	// 플레이어 세팅
//	shared_ptr<PlayerMoveScript> movement = make_shared<PlayerMoveScript>();
//	player = make_shared<APawn>();
//	player->m_szName = L"Player";
//
//	//임시 texture 2 가지
//	shared_ptr<UMaterial> kMat = make_shared<UMaterial>();
//	kMat->Load(L"../Resources/Texture/crow_DIFF.png", L"../Resources/Shader/skinningShader.hlsl");
//	kMat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
//
//	shared_ptr<UMaterial> tMat = make_shared<UMaterial>();
//	tMat->Load(L"../Resources/Texture/red.png", L"../Resources/Shader/Default.hlsl");
//
//	shared_ptr<UMaterial> tCrowMat = make_shared<UMaterial>();
//	tCrowMat->Load(L"../Resources/Texture/crow_DIFF.png", L"../Resources/Shader/Default.hlsl");
//#pragma region Body
//
//	auto bodyRes = meshLoader->GetMeshRes(L"crow_finalbody");
//	shared_ptr<USkinnedMeshComponent> bodyComp = make_shared<USkinnedMeshComponent>();
//	bodyComp->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(bodyRes));
//	bodyComp->SetName(L"Body");
//
//	// ANIM
//	auto animInstance = meshLoader->GetAnimInstance(L"crow_final");
//	bodyComp->SetBaseAnim(animInstance);
//	auto bodyAT = make_shared<AnimTrack>();
//	bodyAT->SetBase(animInstance);
//	bodyComp->SetMeshAnim(bodyAT);
//
//	// MATERIAL
//	bodyComp->SetMaterial(kMat);
//
//	player->SetMeshComponent(bodyComp);
//
//#pragma endregion
//#pragma region BackSocket
//	auto socketRes = meshLoader->GetMeshRes(L"gizmoCenter");
//	shared_ptr<UStaticMeshComponent>  backSocketComp = make_shared<UStaticMeshComponent>();
//	backSocketComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(socketRes));
//	backSocketComp->SetName(L"BackSocket");
//
//	// ANIM
//	backSocketComp->SetAnimInstance(animInstance);
//	backSocketComp->SetTargetBoneIndex(53);
//
//	// MATERIAL
//	backSocketComp->SetMaterial(tMat);
//
//	// TRANSFORM
//	backSocketComp->SetLocalPosition(Vec3(-0.5f, -0.5f, -0.0f));
//	//backSocketComp->SetLocalScale(Vec3(0.01f, 0.01f, 0.01f));
//
//	// PARENT-CHILD
//	bodyComp->AddChild(backSocketComp);
//	backSocketComp->SetParentTransform(dynamic_pointer_cast<USceneComponent>(bodyComp).get());
//	backSocketComp->SetVisible(false);
//#pragma endregion
//#pragma region HandSocket
//	shared_ptr<UStaticMeshComponent>  handSocketComp = make_shared<UStaticMeshComponent>();
//	handSocketComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(socketRes));
//	handSocketComp->SetName(L"HandSocket");
//
//	// ANIM
//	handSocketComp->SetAnimInstance(animInstance);
//	handSocketComp->SetTargetBoneIndex(43);
//
//	// MATERIAL
//	handSocketComp->SetMaterial(tMat);
//
//	// TRANSFORM
//	handSocketComp->SetLocalPosition(Vec3(-0.1f, 0.0f, 0.0f));
//	//handSocketComp->SetLocalScale(Vec3(0.01f, 0.01f, 0.01f));
//	// PARENT-CHILD
//	bodyComp->AddChild(handSocketComp);
//	handSocketComp->SetParentTransform(dynamic_pointer_cast<USceneComponent>(bodyComp).get());
//	handSocketComp->SetVisible(false);
//#pragma endregion
//#pragma region Sword
//	swordComp = make_shared<UStaticMeshComponent>();
//	auto swordRes = meshLoader->GetMeshRes(L"SworddetailSword_weaponTexture1_0");
//	swordComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(swordRes));
//	swordComp->SetName(L"Sword");
//
//	// ANIM
//	// socket의 anim 따라감.
//
//	// MATERIAL
//	swordComp->SetMaterial(tMat);
//
//	// TRANSFORM
//	swordComp->SetLocalScale(Vec3(120.0f, 120.0f, 120.0f));
//	//// BACK POS
//	backSwordRot = Vec3(0.0f, 0.3f, -DD_PI / 2);
//	backSwordPos = Vec3(-0.5f, 0.0f, 0.0f);
//	swordComp->SetLocalPosition(backSwordPos);
//	swordComp->SetLocalRotation(backSwordRot);
//
//	//// HAND POS
//	handSwordRot = Vec3(0.0f, 0.0f, DD_PI / 2);
//	handSwordPos = Vec3(1.0f, 0.0f, 0.0f);
//
//	// PARENT-CHILD
//	backSocketComp->AddChild(swordComp);
//	swordComp->SetParentTransform(dynamic_pointer_cast<USceneComponent>(backSocketComp).get());
//
//#pragma endregion
//	auto cameraComponent = make_shared<UCameraComponent>();
//	cameraComponent->SetLocalPosition(Vec3(20.0f, 20.0f, -20.0f));
//	player->SetCameraComponent(cameraComponent);
//	//player->AddScript(movement);
//	OBJECT->AddActor(player);
	//
	//	AAsset::ExportJsonMesh(player, "player");

#pragma endregion
	//AAsset::LoadJsonMesh("../Resources/Asset/player.mesh.json");

#pragma region Load Player from Json
	{
		//// 플레이어 세팅
		//shared_ptr<PlayerMoveScript> movement = make_shared<PlayerMoveScript>();
		//player = make_shared<APawn>();
		//auto meshComponent = meshLoader->Make("../Resources/Asset/crow_final.mesh.json");
		//player->SetMeshComponent(meshComponent);

		//player->SetPosition(Vec3(10, 10, 10));

#pragma region Sword
		//swordComp = make_shared<UStaticMeshComponent>();
		//auto swordRes = meshLoader->GetMeshRes(L"SworddetailSword_weaponTexture1_0");
		//swordComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(swordRes));
		//swordComp->SetName(L"Sword");
		//auto backSocketComp = player->GetMeshComponent()->GetChildByName(L"BackSocket");
		//// ANIM
		//// socket의 anim 따라감.

		//// MATERIAL
		//swordComp->SetMaterial(tMat);

		//// TRANSFORM
		//swordComp->SetLocalScale(Vec3(120.0f, 120.0f, 120.0f));
		////// BACK POS
		//backSwordRot = Vec3(0.0f, 0.3f, -DD_PI / 2);
		//backSwordPos = Vec3(-0.5f, 0.0f, 0.0f);
		//swordComp->SetLocalPosition(backSwordPos);
		//swordComp->SetLocalRotation(backSwordRot);

		////// HAND POS
		//handSwordRot = Vec3(0.0f, 0.0f, DD_PI / 2);
		//handSwordPos = Vec3(1.0f, 0.0f, 0.0f);

		//// PARENT-CHILD
		//backSocketComp->AddChild(swordComp);
		//swordComp->SetParentTransform(dynamic_pointer_cast<USceneComponent>(backSocketComp).get());

#pragma endregion

		//auto cameraComponent = make_shared<UCameraComponent>();
		//cameraComponent->SetLocalPosition(Vec3(20.0f, 20.0f, -20.0f));
		//player->SetCameraComponent(cameraComponent);
		//player->AddScript(movement);
		//OBJECT->AddActor(player);

		//AAsset::ExportJsonMesh(player, "crow_final");

	}
#pragma endregion
#pragma region HeadRoller
	//enemy1 = make_shared<APawn>();
	//auto meshComponent = meshLoader->Make("../Resources/Asset/E_HEADROLLER.mesh.json");
	//enemy1->SetMeshComponent(meshComponent);
	//enemy1->SetPosition(Vec3(5.0f, 0.0f, 0.0f));
	//OBJECT->AddActor(enemy1);
#pragma endregion
#pragma region BAT
//	// 플레이어 세팅
//	player = make_shared<APawn>();
//	player->m_szName = L"E_Bat";
//
//	// 임시 texture 2 가지
//	shared_ptr<UMaterial> kMat = make_shared<UMaterial>();
//	kMat->Load(L"../Resources/Texture/batTexture.png", L"../Resources/Shader/skinningShader.hlsl");
//	kMat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
//
//	shared_ptr<UMaterial> tMat = make_shared<UMaterial>();
//	tMat->Load(L"../Resources/Texture/batTexture.png", L"../Resources/Shader/Default.hlsl");
//
//	shared_ptr<UMaterial> tCrowMat = make_shared<UMaterial>();
//	tCrowMat->Load(L"../Resources/Texture/crow_DIFF.png", L"../Resources/Shader/Default.hlsl");
//#pragma region Body
//
//	auto bodyRes = meshLoader->GetMeshRes(L"_E_BAT_WhiteBat_White");
//	shared_ptr<USkinnedMeshComponent> bodyComp = make_shared<USkinnedMeshComponent>();
//	bodyComp->SetMesh(dynamic_pointer_cast<USkeletalMeshResources>(bodyRes));
//	bodyComp->SetName(L"Body");
//
//	// ANIM
//	auto animInstance = meshLoader->GetAnimInstance(L"_E_BAT_White");
//	bodyComp->SetBaseAnim(animInstance);
//	auto bodyAT = make_shared<AnimTrack>();
//	bodyAT->SetBase(animInstance);
//	bodyComp->SetMeshAnim(bodyAT);
//	// root
//		int rootBone = bodyComp->GetMesh()->GetBoneIndex(L"_ROOT");
//		animInstance->CheckInPlace(true);
//		animInstance->SetRootIndex(rootBone);
//	// MATERIAL
//	bodyComp->SetMaterial(kMat);
//
//	player->SetMeshComponent(bodyComp);
//
//#pragma endregion
//	auto cameraComponent = make_shared<UCameraComponent>();
//	cameraComponent->SetLocalPosition(Vec3(20.0f, 20.0f, -20.0f));
//	player->SetCameraComponent(cameraComponent);
//	OBJECT->AddActor(player);
//
//	AAsset::ExportJsonMesh(player, "E_BAT_White");
#pragma endregion

#pragma region CAMERA
	// 카메라 세팅
	{
		//m_pCameraActor = make_shared<ACameraActor>();
		//{
		//	m_pCameraActor->SetPosition(player->GetPosition());
		//	m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
		//}
		//CAMERA->Set3DCameraActor(player);
		//OBJECT->AddActor(m_pCameraActor);
	}
#pragma endregion

}
void TestYR::Tick()
{	
	// 애니메이션 확인
	if (INPUT->GetButton(LCLICK))
		//int targetIndex = m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance()->GetAnimIndex(L"Slash_Light_R_new");
		//m_pActor->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance()->PlayOnce(targetIndex);

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

	if (INPUT->GetButton(C))
	{
		auto anim = dynamic_pointer_cast<USkinnedMeshComponent>(player->GetMeshComponent())->GetAnimInstance();
		if (++animIndex >= anim->GetAnimTrackList().size())
		{
			animIndex = 0;
		}
		anim->SetCurrentAnimTrack(animIndex);
	}

	//if (INPUT->GetButton(LCLICK))
	//{
	//	// 부모를 못찾으면 이거는 일회성임 .. 
	//	// 이 처리를 어떻게 깔끔하게 해줘야할지 모르겠음
	//	int index = currentSwordParent->GetChildIndex(swordComp);
	//	if (index < 0)
	//	{
	//		assert(false);
	//	}
	//	currentSwordParent->RemoveChild(index);
	//	handSocketComp->AddChild(swordComp);

	//	// 여전히 일회성
	//	currentSwordParent = handSocketComp;

	//	swordComp->GetMeshTransform()->SetLocalPosition(handSwordPos);
	//	swordComp->GetMeshTransform()->SetLocalRotation(handSwordRot);

	//}
}

void TestYR::Render()
{
}

void TestYR::Destroy()
{
}

//shared_ptr<UMeshComponent> TestYR::MakeMC(MeshComponentData data, bool bRoot, shared_ptr<UAnimInstance> animInstance)
void TestYR::SetupSunLight()
{
	LIGHTMANAGER->Init();

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

void TestYR::SetupEngineCamera()
{
	m_pCameraEngine = make_shared<ACameraActor>();

	m_pCameraEngine->SetPosition({ 0.0f, 10.0f, -10.0f });
	m_pCameraEngine->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraEngine->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(m_pCameraEngine);
	OBJECT->AddActor(m_pCameraEngine);
}