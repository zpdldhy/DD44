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

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());

		CAMERAMANAGER->SetCameraActor(m_pCameraActor);
	}

	{
		m_pActor = make_shared<APawn>();

		m_pStaticMesh = UStaticMeshComponent::CreateCube();
		m_pActor->SetMeshComponent(m_pStaticMesh);
		m_pActor->SetScale({ 5.0f, 5.0f, 5.0f });
		m_pActor->SetPosition({ 0.0f, 2.5f, 10.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		m_pStaticMesh->SetMaterial(material);

		auto pCameraComponent = make_shared<UCameraComponent>();
		pCameraComponent->SetPosition(Vec3(20.f, 20.f, -20.f));
		pCameraComponent->SetNear(10.f);
		pCameraComponent->SetFar(100.f);
		m_pActor->SetCameraComponent(pCameraComponent);

		auto pBoxComponent = make_shared<UBoxComponent>();
		pBoxComponent->SetScale({ 6.f, 6.f, 6.f });
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
		//// Ability
		//{
		//	auto pUIArrowBack = make_shared<AUIActor>();
		//	pUIArrowBack->SetScale(Vec3(169.f, 166.f, 0.f) * 0.6f);
		//	pUIArrowBack->SetPosition(Vec3(-600.f, 370.f, 0.9f));

		//	{
		//		auto pMesh = UStaticMeshComponent::CreatePlane();
		//		pUIArrowBack->SetMeshComponent(pMesh);

		//		auto pMaterial = make_shared<UMaterial>();
		//		pMaterial->Load(L"../Resources/Texture/hud_abilty_box.png", L"../Resources/Shader/Default.hlsl");
		//		pMesh->SetMaterial(pMaterial);
		//	}

		//	pUIArrowBack->Init();
		//	m_vUIs.emplace_back(pUIArrowBack);

		//	auto pUIArrowFrame = make_shared<AUIActor>();
		//	pUIArrowFrame->SetScale(Vec3(182.f, 181.f, 0.f) * 0.6f);
		//	pUIArrowFrame->SetPosition(pUIArrowBack->GetPosition() + Vec3(0.f, 0.f, -0.1f));

		//	{
		//		auto pMesh = UStaticMeshComponent::CreatePlane();
		//		pUIArrowFrame->SetMeshComponent(pMesh);

		//		auto pMaterial = make_shared<UMaterial>();
		//		pMaterial->Load(L"../Resources/Texture/hud_abilty_frame_chipped.png", L"../Resources/Shader/Default.hlsl");
		//		pMesh->SetMaterial(pMaterial);
		//	}

		//	pUIArrowFrame->Init();
		//	m_vUIs.emplace_back(pUIArrowFrame);

		//	auto pUIArrow = make_shared<AUIActor>();
		//	pUIArrow->SetScale(Vec3(107.f, 108.f, 0.f) * 0.4f);
		//	pUIArrow->SetPosition(pUIArrowBack->GetPosition() + Vec3(-5.f, -5.f, -0.1f));

		//	{
		//		auto pMesh = UStaticMeshComponent::CreatePlane();
		//		pUIArrow->SetMeshComponent(pMesh);

		//		auto pMaterial = make_shared<UMaterial>();
		//		pMaterial->Load(L"../Resources/Texture/Icon_Arrow.png", L"../Resources/Shader/Default.hlsl");
		//		pMesh->SetMaterial(pMaterial);
		//	}

		//	pUIArrow->Init();
		//	m_vUIs.emplace_back(pUIArrow);
		//}

		//// Energe
		//{
		//	auto pUIEnergyBack = make_shared<AUIActor>();
		//	pUIEnergyBack->SetScale(Vec3(64.f, 64.f, 0.f) * 0.5f);
		//	pUIEnergyBack->SetPosition(Vec3(-500.f, 360.f, 0.9f));

		//	{
		//		auto pMesh = UStaticMeshComponent::CreatePlane();
		//		pUIEnergyBack->SetMeshComponent(pMesh);

		//		auto pMaterial = make_shared<UMaterial>();
		//		pMaterial->Load(L"../Resources/Texture/MP_Box.png", L"../Resources/Shader/Default.hlsl");
		//		pMesh->SetMaterial(pMaterial);
		//	}

		//	pUIEnergyBack->Init();
		//	m_vUIs.emplace_back(pUIEnergyBack);

		//	auto pUIEnergy = make_shared<AUIActor>();
		//	pUIEnergy->SetScale(Vec3(64.f, 64.f, 0.f) * 0.5f);
		//	pUIEnergy->SetPosition(pUIEnergyBack->GetPosition() + Vec3(0.f, 0.f, -0.1f));

		//	{
		//		auto pMesh = UStaticMeshComponent::CreatePlane();
		//		pUIEnergy->SetMeshComponent(pMesh);

		//		auto pMaterial = make_shared<UMaterial>();
		//		pMaterial->Load(L"../Resources/Texture/MP_Blip.png", L"../Resources/Shader/Default.hlsl");
		//		pMesh->SetMaterial(pMaterial);
		//	}

		//	pUIEnergy->Init();
		//	m_vUIs.emplace_back(pUIEnergy);
		//}
	}

	GUI->SetCharacterEditorCallback(
		[this](int actorType, int compType, const char* assetPath, Vec3 pos, Vec3 rot, Vec3 scale, int scriptType)
		{
			auto loader = std::make_shared<ActorLoader>();
			auto actor = loader->LoadOne(assetPath);
			if (!actor) return;

			actor->SetPosition(pos);
			actor->SetRotation(rot);
			actor->SetScale(scale);

			auto cam = std::make_shared<UCameraComponent>();
			cam->SetPosition(Vec3(10, 10, -10));
			actor->SetCameraComponent(cam);

			if (scriptType == 1)
				actor->AddScript(std::make_shared<PlayerMoveScript>());
			else if (scriptType == 2)
				//actor->AddScript(std::make_shared<EnemyAIScript>());

			actor->Init();
			OBJECTMANAGER->AddActor(actor);
		}
	);

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
	CAMERAMANAGER->SetCameraActor(m_pCameraActor);
	if (INPUT->GetButtonDown(O))
		CAMERAMANAGER->SetCameraActor(m_pActor);

	if (INPUT->GetButton(I))
	{
		m_pActor->SetDelete(true);
		m_vUIs[0]->SetDelete(true);
		m_vUIs[1]->SetDelete(true);
		m_vUIs[2]->SetDelete(true);
	}

	//UIMANAGER->SetUIList(m_vUIs);

	// 하나의 actor 조작 가능
	{
		//if (ImGui::Button("Load Asset"))
		//{
		//	const char* meshNames[] = { "crow", "eye", "sword", "sword_effect1", "sword_effect2", "sword_effect3" };

		//	m_pLoader = std::make_shared<ActorLoader>();
		//	m_pLoader->LoadOne("../Resources/Asset/crow_final.asset");

		//	m_vMeshList = m_pLoader->LoadMesh();
		//	m_iSelectedMeshIndex = 0;

		//	m_vMeshLabelPtrs.clear();
		//	for (int i = 0; i < m_vMeshList.size(); ++i)
		//	{
		//		m_vMeshLabelPtrs.push_back(meshNames[i]);
		//	}

		//	// 애니메이션도 함께 로드
		//	m_pLoader->LoadAnim();
		//	m_iSelectedAnimIndex = 0;
		//}

		//if (!m_vMeshList.empty())
		//{
		//	ImGui::Combo("Mesh", &m_iSelectedMeshIndex, m_vMeshLabelPtrs.data(), (int)m_vMeshLabelPtrs.size());

		//	// 애니메이션 간단 리스트
		//	if (!m_pLoader->m_vAnimInstanceList.empty())
		//	{
		//		auto animInstance = m_pLoader->m_vAnimInstanceList[0];
		//		const auto& animList = animInstance->GetAnimTrackList();

		//		static std::vector<std::string> animNames;
		//		static std::vector<const char*> animNamePtrs;

		//		animNames.clear();
		//		animNamePtrs.clear();

		//		for (const auto& anim : animList)
		//		{
		//			std::wstring wname = anim.m_szName;
		//			std::string name(wname.begin(), wname.end());
		//			animNames.push_back(name);
		//			animNamePtrs.push_back(animNames.back().c_str());
		//		}

		//		ImGui::Combo("Anim", &m_iSelectedAnimIndex, animNamePtrs.data(), (int)animNamePtrs.size());
		//		ImGui::SliderFloat("Anim Speed", &animInstance->m_fAnimPlayRate, 1.0f, 60.0f, "%.1f");
		//	}

		//	if (ImGui::Button("Create Actor"))
		//	{
		//		m_pPreviewActor = std::make_shared<APawn>();
		//		auto selectedMesh = m_vMeshList[m_iSelectedMeshIndex];
		//		m_pPreviewActor->SetMeshComponent(selectedMesh);

		//		// 애니메이션 적용
		//		if (!m_pLoader->m_vAnimInstanceList.empty())
		//		{
		//			auto anim = m_pLoader->m_vAnimInstanceList[0];
		//			anim->SetCurrentAnimTrack(m_iSelectedAnimIndex);

		//			auto skinned = std::dynamic_pointer_cast<USkinnedMeshComponent>(selectedMesh);
		//			if (skinned)
		//			{
		//				//skinned->SetAnimInstance(anim);
		//			}
		//		}

		//		m_pPreviewActor->SetPosition(Vec3(20.f, 0.f, 0.f));
		//		m_pPreviewActor->SetScale(Vec3(10.f, 10.f, 10.f));
		//		OBJECTMANAGER->AddActor(m_pPreviewActor);
		//	}
		//}
	}

	// 다수의 actor 생성 및 bone 붙이기
	{
		if (ImGui::Button("Load Asset"))
		{
			const char* meshNames[] = { "crow", "eye", "sword", "sword_effect1", "sword_effect2", "sword_effect3" };

			m_pLoader = std::make_shared<ActorLoader>();
			m_pLoader->LoadOne("../Resources/Asset/crow_final.asset");

			m_vMeshList = m_pLoader->LoadMesh();
			m_iSelectedMeshIndex = 0;

			m_vMeshLabelPtrs.clear();
			for (int i = 0; i < m_vMeshList.size(); ++i)
			{
				m_vMeshLabelPtrs.push_back(meshNames[i]);
			}

			// 애니메이션도 함께 로드
			m_pLoader->LoadAnim();
			m_iSelectedAnimIndex = 0;
		}

		if (!m_vMeshList.empty())
		{
			const char* compTypes[] = { "Skinned", "Static" };

			ImGui::Combo("Mesh", &m_iSelectedMeshIndex, m_vMeshLabelPtrs.data(), (int)m_vMeshLabelPtrs.size());
			ImGui::Combo("Component Type", &m_iSelectedCompType, compTypes, IM_ARRAYSIZE(compTypes));

			// 애니메이션 간단 리스트
			if (!m_pLoader->m_vAnimInstanceList.empty())
			{
				auto animInstance = m_pLoader->m_vAnimInstanceList[0];
				const auto& animList = animInstance->GetAnimTrackList();

				static std::vector<std::string> animNames;
				static std::vector<const char*> animNamePtrs;

				animNames.clear();
				animNamePtrs.clear();

				for (const auto& anim : animList)
				{
					std::wstring wname = anim.m_szName;
					std::string name(wname.begin(), wname.end());
					animNames.push_back(name);
					animNamePtrs.push_back(animNames.back().c_str());
				}

				ImGui::Combo("Anim", &m_iSelectedAnimIndex, animNamePtrs.data(), (int)animNamePtrs.size());
				ImGui::SliderFloat("Anim Speed", &animInstance->m_fAnimPlayRate, 1.0f, 60.0f, "%.1f");
			}
		}

		if (ImGui::Button("Create Actor"))
		{
			auto actor = std::make_shared<APawn>();
			auto selectedMesh = m_vMeshList[m_iSelectedMeshIndex];

			std::shared_ptr<UMeshComponent> meshComp;

			if (m_iSelectedCompType == 0) // Skinned
			{
				meshComp = std::make_shared<USkinnedMeshComponent>();
				auto skinned = std::dynamic_pointer_cast<USkinnedMeshComponent>(meshComp);

				//skinned->CopyFrom(selectedMesh); // 복사 함수 직접 구현 필요

				if (!m_pLoader->m_vAnimInstanceList.empty())
				{
					auto anim = m_pLoader->m_vAnimInstanceList[0];
					anim->SetCurrentAnimTrack(m_iSelectedAnimIndex);
					//skinned->SetAnimInstance(anim);
				}
			}
			else // Static
			{
				meshComp = std::make_shared<UStaticMeshComponent>();
				auto staticComp = std::dynamic_pointer_cast<UStaticMeshComponent>(meshComp);
				//staticComp->CopyFrom(selectedMesh); // 역시 복사 함수 필요
			}

			//if (!m_vMeshList.empty() && m_iSelectedMeshIndex == 0) // crow만 skinned
			//{
			//	auto skinned = std::dynamic_pointer_cast<USkinnedMeshComponent>(m_vMeshList[0]);
			//	if (skinned)
			//	{
			//		const auto& boneList = skinned->GetBoneList();

			//		m_vBoneLabels.clear();
			//		m_vBoneLabelPtrs.clear();

			//		for (const auto& bone : boneList)
			//		{
			//			std::wstring wname = bone.m_szName;
			//			std::string name(wname.begin(), wname.end());
			//			m_vBoneLabels.push_back(name);
			//			m_vBoneLabelPtrs.push_back(m_vBoneLabels.back().c_str());
			//		}

			//		ImGui::Combo("Attach Bone", &m_iSelectedBoneIndex, m_vBoneLabelPtrs.data(), (int)m_vBoneLabelPtrs.size());
			//	}
			//}

			actor->SetMeshComponent(selectedMesh);
			actor->SetScale(Vec3(10.f, 10.f, 10.f));
			actor->Init();
			m_vPreviewActors.push_back(actor);
			OBJECTMANAGER->AddActorList(m_vPreviewActors);
		}
	}
}

void TestSY::Render()
{	
	if (m_bEditorWireframe)
	{
		DC->RSSetState(STATE->m_pRSWireFrame.Get());
	}
	else
	{
		DC->RSSetState(STATE->m_pRSSolid.Get());
	}
}

void TestSY::Destroy()
{
}