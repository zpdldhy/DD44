#include "pch.h"
#include "TestSY.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "APawn.h"
#include "ACameraActor.h"
#include "ASky.h"
#include "EngineCameraMoveScript.h"
#include "kkongchiMoveScript.h"
#include "UTerrainMeshComponent.h"
#include "UMeshResources.h"
#include "CameraManager.h"
#include "Input.h"
#include <random>
#include "ATerrainTileActor.h"
#include "ImGuiCore.h"
#include "DxState.h"
#include "MapEditorUI.h"
#include "UBoxComponent.h"
#include "AUIActor.h"
#include "UIManager.h"
#include "ObjectLoader.h"
#include "ACharacter.h"
#include "ObjectManager.h"
#include "ActorLoader.h"
#include "PlayerMoveScript.h"
#include "UAnimInstance.h"
#include "USkinnedMeshComponent.h"
#include "ActorLoader.h"
#include "USkeletalMeshResources.h"
#include "PrefabLoader.h"
#include "Timer.h"
#include "Functions.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
		m_pCameraActor->SetActorName(L"EnginCamera");

		CAMERAMANAGER->Set3DCameraActor(m_pCameraActor);
	}

	{
		m_pActor = make_shared<APawn>();
		m_pActor->SetActorName(L"kkongchi");

		m_pStaticMesh = UStaticMeshComponent::CreateCube();
		m_pActor->SetMeshComponent(m_pStaticMesh);
		m_pActor->SetScale({ 5.0f, 5.0f, 5.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		m_pStaticMesh->SetMaterial(material);

		auto pCameraComponent = make_shared<UCameraComponent>();
		pCameraComponent->SetLocalPosition(Vec3(20.f, 0.f, -0.f));
		pCameraComponent->SetNear(10.f);
		pCameraComponent->SetFar(100.f);
		m_pActor->SetCameraComponent(pCameraComponent);

		auto pBoxComponent = make_shared<UBoxComponent>();
		pBoxComponent->SetLocalScale({ 6.f, 6.f, 6.f });
		m_pActor->SetShapeComponent(pBoxComponent);

		m_pActor->AddScript(make_shared<kkongchiMoveScript>());
	}

	{
		m_pSky = make_shared<ASky>();

		m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
		m_pSky->SetMeshComponent(m_pSkyMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
		m_pSkyMesh->SetMaterial(material);
	}

	// UIs
	{
		// Ability
		{
			auto pUIArrowBack = make_shared<AUIActor>();
			pUIArrowBack->SetScale(Vec3(169.f, 166.f, 0.f) * 0.6f);
			pUIArrowBack->SetPosition(Vec3(-600.f, 370.f, 0.9f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIArrowBack->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/hud_abilty_box.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UIMANAGER->AddUI(pUIArrowBack);

			auto pUIArrowFrame = make_shared<AUIActor>();
			pUIArrowFrame->SetScale(Vec3(182.f, 181.f, 0.f) * 0.6f);
			pUIArrowFrame->SetPosition(pUIArrowBack->GetPosition() + Vec3(0.f, 0.f, -0.1f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIArrowFrame->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/hud_abilty_frame_chipped.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UIMANAGER->AddUI(pUIArrowFrame);

			auto pUIArrow = make_shared<AUIActor>();
			pUIArrow->SetScale(Vec3(107.f, 108.f, 0.f) * 0.4f);
			pUIArrow->SetPosition(pUIArrowBack->GetPosition() + Vec3(-5.f, -5.f, -0.1f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIArrow->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/Icon_Arrow.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UIMANAGER->AddUI(pUIArrow);
		}

		// Energe
		{
			auto pUIEnergyBack = make_shared<AUIActor>();
			pUIEnergyBack->SetScale(Vec3(64.f, 64.f, 0.f) * 0.5f);
			pUIEnergyBack->SetPosition(Vec3(-500.f, 360.f, 0.9f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIEnergyBack->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/MP_Box.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UIMANAGER->AddUI(pUIEnergyBack);

			auto pUIEnergy = make_shared<AUIActor>();
			pUIEnergy->SetScale(Vec3(64.f, 64.f, 0.f) * 0.5f);
			pUIEnergy->SetPosition(pUIEnergyBack->GetPosition() + Vec3(0.f, 0.f, -0.1f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIEnergy->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/MP_Blip.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UIMANAGER->AddUI(pUIEnergy);
		}
	}

	GUI->SetCharacterEditorCallback(
		[](std::shared_ptr<UMeshComponent> rootComponent, const Vec3& position, const Vec3& rotation, const Vec3& scale, int scriptType)
		{
			if (!rootComponent)
			{
				return;
			}

			auto actor = std::make_shared<AActor>();
			actor->SetMeshComponent(rootComponent);

			actor->SetPosition(position);
			actor->SetRotation(rotation);
			actor->SetScale(scale);

			if (scriptType == 1) actor->AddScript(std::make_shared<PlayerMoveScript>());
			//if (scriptType == 2) actor->AddScript(std::make_shared<EnemyAIScript>());

			auto cam = std::make_shared<UCameraComponent>();
			cam->SetLocalPosition(Vec3(10, 10, -10));
			actor->SetCameraComponent(cam);

			OBJECTMANAGER->AddActor(actor);
		});

	GUI->SetMapEditorCallback([this]()
	{
		MapEditorUI* editor = GUI->GetMapEditorUI();
		if (!editor) return;

		auto tile = std::make_shared<ATerrainTileActor>();

		tile->m_iNumCols = editor->GetNumCols();
		tile->m_iNumRows = editor->GetNumRows();
		tile->m_fCellSize = editor->GetCellSize();

		tile->CreateTerrain(editor->GetTexturePath(), editor->GetShaderPath());
		tile->SetPosition(editor->GetPosition());
		tile->SetRotation(editor->GetRotation());
		tile->SetScale(editor->GetScale());

		OBJECTMANAGER->AddActor(tile);
	});

	GUI->SetObjectEditorCallback([this](int actorType, int meshType, const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale)
	{
		AssimpLoader loader;
		vector<MeshData> meshList = loader.Load(objPath);
		if (meshList.empty())
			return;

		// Mesh Component 생성
		shared_ptr<UMeshComponent> meshComp;
		if (meshType == 0)
		{
			// SkinnedMeshComponent 설정 시 향후 확장
			meshComp = make_shared<UStaticMeshComponent>();

			auto meshRes = make_shared<UStaticMeshResources>();
			meshRes->SetVertexList(meshList[0].m_vVertexList);
			meshRes->SetIndexList(meshList[0].m_vIndexList);
			meshRes->Create();

			dynamic_cast<UStaticMeshComponent*>(meshComp.get())->SetMesh(meshRes);
		}
		else if (meshType == 1)
		{
			meshComp = make_shared<UStaticMeshComponent>();

			auto meshRes = make_shared<UStaticMeshResources>();
			meshRes->SetVertexList(meshList[0].m_vVertexList);
			meshRes->SetIndexList(meshList[0].m_vIndexList);
			meshRes->Create();

			dynamic_cast<UStaticMeshComponent*>(meshComp.get())->SetMesh(meshRes);
		}

		// 머티리얼 설정
		auto mat = make_shared<UMaterial>();
		mat->Load(
			std::wstring(texPath, texPath + strlen(texPath)),
			std::wstring(shaderPath, shaderPath + strlen(shaderPath))
		);
		meshComp->SetMaterial(mat);

		// Actor 생성
		shared_ptr<AActor> actor;
		if (actorType == 0)
		{
			actor = make_shared<APawn>(); // ACharacter로 교체 가능
		}
		else if (actorType == 1)
		{
			actor = make_shared<APawn>(); // AEnemy로 교체 가능
		}
		else if (actorType == 2)
		{
			actor = make_shared<APawn>(); // AObject로 교체 가능
		}

		actor->SetMeshComponent(meshComp);
		actor->SetPosition(pos);
		actor->SetRotation(rot);
		actor->SetScale(scale);

		OBJECTMANAGER->AddActor(actor);
	});

	OBJECTMANAGER->AddActor(m_pActor);
	OBJECTMANAGER->AddActor(m_pCameraActor);
	OBJECTMANAGER->AddActor(m_pSky);	
}

void TestSY::Update()
{
	CAMERAMANAGER->Set3DCameraActor(m_pCameraActor);
	if (INPUT->GetButtonDown(O))
		CAMERAMANAGER->Set3DCameraActor(m_pActor);

	if (INPUT->GetButton(I))
	{
		m_pActor->SetDelete(true);
	}

	// Mouse Picking
	if (INPUT->GetButton(LCLICK))
		SetClickPos();
}

void TestSY::Render()
{	

}

void TestSY::Destroy()
{
}

void TestSY::SetClickPos()
{
	POINT mousePos = INPUT->GetMousePos();
	float deltaTime = TIMER->GetDeltaTime() * 10.f;

	float fWinSizeX = static_cast<float>(g_windowSize.x);
	float fWinSizeY = static_cast<float>(g_windowSize.y);

	// To NDC
	Vec3 vMouseEnd(0.f, 0.f, 1.f);
	vMouseEnd.x = (2.f * static_cast<float>(mousePos.x) / fWinSizeX) - 1.f;
	vMouseEnd.y = 1.f - (2.f * static_cast<float>(mousePos.y) / fWinSizeY);

	Matrix mProjViewInvert = Matrix::Identity;
	(CAMERAMANAGER->Get3DView() * CAMERAMANAGER->Get3DProjection()).Invert(mProjViewInvert);

	vMouseEnd = Vec3::Transform(vMouseEnd, mProjViewInvert);

	m_vMouseRay.position = CAMERAMANAGER->Get3DCameraComponent()->GetCameraPos();
	m_vMouseRay.direction = vMouseEnd - m_vMouseRay.position;

	Vec3 vMouseMiddle = (vMouseEnd + m_vMouseRay.position) / 2.f;

	auto pActor = make_shared<APawn>();

	auto pMesh = UStaticMeshComponent::CreateRay();
	pMesh->GetMesh()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	pActor->SetMeshComponent(pMesh);
	pActor->SetPosition(vMouseMiddle);
	//pActor->SetRotation(Vec3(0.f, DD_PI / 2.f, 0.f));
	pActor->SetScale(Vec3(100.f, 100.f, 100.f));

	auto pMaterial = make_shared<UMaterial>();
	pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");	
	pMesh->SetMaterial(pMaterial);

	OBJECTMANAGER->AddActor(pActor);
}
