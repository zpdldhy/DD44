#include "pch.h"
#include "TestSJ.h"
#include "Input.h"
#include "DxWrite.h"
#include "Sound.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "ASky.h"
#include "APawn.h"
#include "ACameraActor.h"
#include "Timer.h"
#include "ImGuiCore.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "DxState.h"
#include "ALight.h"
#include "ActorLoader.h"
#include "ObjectManager.h"
#include "LightManager.h"
#include "PostProcessManager.h"
#include "AssimpLoader.h"
#include "ATerrainTileActor.h"
#include "UBoxComponent.h"
#include "PlayerMoveScript.h"

void TestSJ::Init()
{
	// Asset 로딩 ( 변경하면서 콜백/LoadPrefab이 많이 바껴서 실행할 때 오류나더라고. 그래서 main꺼에서 좀 복사해뒀습니다 )
	actorLoader.LoadAllAsset();
	meshLoader.SetMesh(actorLoader.LoadMeshMap());
	meshLoader.SetAnim(actorLoader.LoadAnimMap());


	SetupObjectEditorCallback();
	LoadAllPrefabs(".object.json");

	SOUNDMANAGER->LoadAllSounds();

	LIGHTMANAGER->Init();

	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, -15.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}

	{
		m_pActor = make_shared<APawn>();
		m_pStaticMesh = UStaticMeshComponent::CreateCube();
		//m_pStaticMesh = UStaticMeshComponent::CreateSphere(20, 20);
		m_pActor->SetMeshComponent(m_pStaticMesh);
		m_pActor->SetScale({ 5.0f, 5.0f, 5.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/PREffect.hlsl");

		material->SetDiffuseParams(Vec3(1.0f, 1.0f, 1.0f), 1.0f);
		material->SetAmbientParams(Vec3(0.5f, 0.5f, 0.5f), 1.0f);
		material->SetSpecularParams(Vec3(1.0f, 1.0f, 1.0f), 64.0f);
		material->SetEmissiveParams(Vec3(0, 0, 0), 0.0f);


		m_pStaticMesh->SetMaterial(material);
		
	}

	{
		m_pActor2 = make_shared<APawn>();

		m_pStaticMesh2 = UStaticMeshComponent::CreateSphere(20, 20);


		m_pActor2->SetMeshComponent(m_pStaticMesh2);
		m_pActor2->SetScale({ 1.0f, 1.0f, 1.0f });
		m_pActor2->SetPosition({ 5.0f, 0.0f, 10.0f }); // 첫 번째 큐브 옆에 배치
		m_pActor2->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material2 = make_shared<UMaterial>();
		material2->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/PREffect.hlsl");
		material2->SetUseStencil(true);
		
		m_pStaticMesh2->SetMaterial(material2);
		
	}

	{
		m_pSky = make_shared<ASky>();

		m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
		m_pSky->SetMeshComponent(m_pSkyMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
		m_pSkyMesh->SetMaterial(material);
	}

	

	

	CAMERA->Set3DCameraActor(m_pCameraActor);

	GUI->SetEffectEditorCallback(
		[this](int selected, float glowPower, Vec3 glowColor, Vec3 emissiveColor, float emissivePower) {
			if (selected == 0 && m_pStaticMesh)
			{
				targetMat = m_pStaticMesh->GetMaterial();
			}
			else if (selected == 1 && m_pStaticMesh2)
			{
				targetMat = m_pStaticMesh2->GetMaterial();
			}

			if (targetMat)
			{
				targetMat->SetGlowParams(glowPower, glowColor);
				targetMat->SetEmissiveParams(emissiveColor, emissivePower);
			}
		}
	);

	GUI->SetLightEditorCallback(
		[this](ELightType lightType, Vec3 lightColor, float intensity, Vec3 ambientColor, float ambientPower)
		{
			if (m_pLight && m_pLight->GetLightComponent())
			{
				m_pLight->GetLightComponent()->SetLightType(static_cast<ELightType>(lightType));
				m_pLight->GetLightComponent()->SetColor(lightColor);
				m_pLight->GetLightComponent()->SetIntensity(intensity);
				m_pLight->GetLightComponent()->SetAmbientColor(ambientColor);
				m_pLight->GetLightComponent()->SetAmbientPower(ambientPower);
			}
		}
	);

	// 메쉬 파싱
	{
		m_pLoader = make_shared<ActorLoader>();
		m_pLoader->LoadOne("../Resources/Asset/crow_final.asset");
		m_vMeshList = m_pLoader->LoadMesh();
		m_vMeshList[2]->GetMaterial()->SetEmissiveParams(Vec3(5.0, 0, 0), 1.f);
		// 2번 인덱스가 검. meshComponent 타고타고 UObject의 이름 확인해보면, "detailSword_weaponTexuture1".
		m_pSwordActor = make_shared<APawn>();
		m_pSwordActor->SetMeshComponent(m_vMeshList[2]);
		m_pSwordActor->SetPosition(Vec3(20.f, 0.0f, 0.0f));
		m_pSwordActor->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	}



	{
		m_pLight = make_shared<ALight>();
		m_pLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
		m_pLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
		m_pLight->GetLightComponent()->SetAmbientPower(0.3f);

	}

	{
		m_pPointLight = make_shared<ALight>();

		m_pPointLight->SetPosition(Vec3(10.0f, 10.0f, 10.0f)); // 눈에 띄도록 위로 띄움

		auto lightComp = m_pPointLight->GetLightComponent();
		lightComp->SetLightType(ELightType::Point);
		lightComp->SetColor(Vec3(0.0f, 0.3f, 1.0f));     // 파란빛
		lightComp->SetIntensity(2.0f);
		lightComp->SetRange(20.0f);                      // 빛 퍼짐 정도
	}

	OBJECT->AddActor(m_pPointLight);
	OBJECT->AddActor(m_pLight);
	OBJECT->AddActor(m_pCameraActor);
	OBJECT->AddActor(m_pActor);
	OBJECT->AddActor(m_pActor2);
	OBJECT->AddActor(m_pSky);
	//OBJECT->AddActor(m_pSwordActor);

	LIGHTMANAGER->Clear();
	LIGHTMANAGER->RegisterLight(m_pLight);
	LIGHTMANAGER->RegisterLight(m_pPointLight); 
}

void TestSJ::Update()
{
	m_pLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
	//LIGHTMANAGER->UpdateLightCB();
	// 오브젝트 회전
	{
		static float angle = 0.0f;
		angle += TIMER->GetDeltaTime();
		angle = fmodf(angle, DD_PI * 2);

		float radius = 6.0f;
		float centerX = m_pActor->GetPosition().x;
		float centerZ = m_pActor->GetPosition().z;

		float x = centerX + radius * cosf(angle);
		float z = centerZ + radius * sinf(angle);

		Vec3 newPos = { x, 0.0f, z };
		m_pActor2->SetPosition(newPos);
	}
	//Rim Light
	/*if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
	{
		Vec3 camPos = m_pCameraActor->GetCameraComponent()->GetWorldPosition();
		m_pStaticMesh->GetMaterial()->SetCameraPos(camPos);
		m_pStaticMesh2->GetMaterial()->SetCameraPos(camPos);
	}*/
	//Emissive
	{
		//m_pStaticMesh2->GetMaterial()->SetEmissiveParams(Vec3(0.0f, 1.0f, 0.0f), 0.1f);

	}
	//UVDistortion
	//{
	//	static bool bUVInitialized = false;
	//	if (!bUVInitialized)
	//	{
	//		m_pStaticMesh->GetMaterial()->SetUVDistortionParams(0.08f, 1.0f, 15.0f); // strength, speed, frequency
	//		m_pStaticMesh2->GetMaterial()->SetUVDistortionParams(0.01f, 1.5f, 8.0f);  // optional: 다른 효과
	//		bUVInitialized = true;
	//	}

	//	float delta = TIMER->GetDeltaTime();

	//	if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
	//	{
	//		m_pStaticMesh->GetMaterial()->UpdateUVDistortionBuffer(delta);
	//	}
	//	if (m_pStaticMesh2 && m_pStaticMesh2->GetMaterial())
	//	{
	//		m_pStaticMesh2->GetMaterial()->UpdateUVDistortionBuffer(delta);
	//	}
	//}


	{
		/*shared_ptr<UMaterial> targetMat = nullptr;
	static int prevSelected = -1;
	if (GUI->m_iSelectedActor != prevSelected)
	{
		prevSelected = GUI->m_iSelectedActor;
		GUI->m_bDidInitialApply = false;
		targetMat = m_pStaticMesh->GetMaterial();
		if (prevSelected == 0 && m_pStaticMesh)
		{
			GUI->SetInitialMaterialValues(m_pStaticMesh->GetMaterial());
		}
		else if (prevSelected == 1 && m_pStaticMesh2)
		{
			GUI->SetInitialMaterialValues(m_pStaticMesh2->GetMaterial());
		}
		return;
	}*/


	/*if (GUI->m_iSelectedActor == 0 && m_pStaticMesh)
	{
		targetMat = m_pStaticMesh->GetMaterial();
	}
	else if (GUI->m_iSelectedActor == 1 && m_pStaticMesh2)
	{
		targetMat = m_pStaticMesh2->GetMaterial();
	}


	if(targetMat)
	{
		targetMat->SetGlowParams(GUI->m_fGlowPower, GUI->m_vGlowColor);
		targetMat->SetDissolveParams(GUI->m_fDissolveThreshold);
	}*/


	}

	// fallback 제거 가능 (선택이 명확하니까)
	//GUI->SetTargetMaterial(targetMat);
	//Glow
	{
		//static bool bGlow = false;
		//static float glowTimer = 0.0f;
		//const float maxGlowDuration = 0.5f;

		//// R 키를 누르면 발광 시작
		//if (INPUT->GetButtonDown(R))
		//{
		//	bGlow = true;
		//	glowTimer = maxGlowDuration;
		//}

		//float glowPower = 0.0f;

		//if (bGlow)
		//{
		//	glowTimer -= TIMER->GetDeltaTime();

		//	float ratio = glowTimer / maxGlowDuration; // 1.0 → 0.0 
		//	glowPower = 1.0f;//ratio*5.0f;
		//	
		//	if (glowTimer <= 0.0f)
		//	{
		//		bGlow = false;
		//		glowTimer = 0.0f;
		//	}
		//}

		//if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
		//{
		//	m_pStaticMesh->GetMaterial()->SetGlowParams(
		//		GUI->GetGlowPower(),//glowPower,
		//		GUI->GetGlowColor()
		//	);
		//}

	}
	//Dissolve
	{
		/*if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
		{
			m_pStaticMesh->GetMaterial()->SetDissolveParams(GUI->GetDissolveThreshold());
		}*/

	}
	//Flesh
	{
		//static float flashTimer = 0.0f;

		//if (INPUT->GetButtonDown(R)) // 피격 가정
		//{
		//	flashTimer = 1.0f; // Flash 시작
		//}

		//flashTimer -= TIMER->GetDeltaTime() * 3.0f; // 빠르게 사라지게
		//flashTimer = max(flashTimer, 0.0f);

		//if (targetMat)
		//{
		//	targetMat->SetHitFlashTime(flashTimer);
		//}

		//static float flashTimer = 0.0f;

		//if (INPUT->GetButtonDown(R))
		//	flashTimer = 1.0f;

		//flashTimer -= TIMER->GetDeltaTime() * 3.0f;
		//flashTimer = max(flashTimer, 0.0f);

		//int selected = GUI->GetEffectEditorUI()->GetSelectedActor(); // 또는 따로 selected 값 가져오기

		//if (selected == 0 && m_pStaticMesh)
		//	m_pStaticMesh->GetMaterial()->SetHitFlashTime(flashTimer);
		//else if (selected == 1 && m_pStaticMesh2)
		//	m_pStaticMesh2->GetMaterial()->SetHitFlashTime(flashTimer);
		//else if (selected == 2)
		//	m_vMeshList[2]->GetMaterial()->SetHitFlashTime(flashTimer);
		//

		static float angle = 0.0f;
		angle += TIMER->GetDeltaTime();
		angle = fmodf(angle, DD_PI * 2);


		float speed = 0.7f;
		angle += TIMER->GetDeltaTime() * speed * DD_PI * 2.0f; // 속도 * 각속도
		//angle = fmodf(angle, DD_PI * 2);
		//float flashTimer = (sin(angle) + 1.0f) * 0.5f;
		float flashTimer = pow((sin(angle) + 1.0f) * 0.5f, 0.25f);

		// 적용
		m_vMeshList[2]->GetMaterial()->SetHitFlashTime(flashTimer);
	}
	//Sound
	/*{
		SOUNDMANAGER->GetPtr(ESoundType::Bgm)->Play2D();
		if (INPUT->GetButton(A))
		{
			SOUNDMANAGER->GetPtr(ESoundType::Bomb)->PlayEffect2D();
		}
	}*/
	//Write
	{
		/*if (INPUT->GetButton(A))
	{
		DXWRITE->SetFont(L"소야뜰9");
	}

	if (INPUT->GetButton(S))
	{
		DXWRITE->SetFont(L"궁서");
	}

	if (INPUT->GetButton(D))
	{
		DXWRITE->SetFont(L"Rubik-Beastly");
	}

	if (INPUT->GetButton(W))
	{
		DXWRITE->SetFont(L"Mapo홍대프리덤");
	}

	if (INPUT->GetButton(Q))
	{
		DXWRITE->IncreaseFontSize();
	}
	if (INPUT->GetButton(E))
	{
		DXWRITE->DecreaseFontSize();
	}

	if (INPUT->GetButton(G))
	{
		DXWRITE->SetFontColor(D2D1::ColorF(D2D1::ColorF::Orange));
	}
	if (INPUT->GetButton(H))
	{
		DXWRITE->SetFontColor(D2D1::ColorF(D2D1::ColorF::Aqua));
	}
	if (INPUT->GetButton(J))
	{
		DXWRITE->SetFontColor(D2D1::ColorF(D2D1::ColorF::DarkSeaGreen));
	}

	if (INPUT->GetButton(R))
	{
		DXWRITE->SetAlignment(DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	if (INPUT->GetButton(T))
	{
		DXWRITE->SetAlignment(
			DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
	if (INPUT->GetButton(Y))
	{
		DXWRITE->GetInstance()->SetAlignment(
			DWRITE_TEXT_ALIGNMENT_TRAILING,
			DWRITE_PARAGRAPH_ALIGNMENT_FAR);
	}*/
	}
	if (INPUT->GetButton(U)) // X- (왼쪽)
		m_pPointLight->AddPosition(Vec3(-0.1f, 0.f, 0.f));
	if (INPUT->GetButton(O)) // X+ (오른쪽)
		m_pPointLight->AddPosition(Vec3(0.1f, 0.f, 0.f));

	if (INPUT->GetButton(J)) // Y- (아래)
		m_pPointLight->AddPosition(Vec3(0.f, -0.1f, 0.f));
	if (INPUT->GetButton(L)) // Y+ (위)
		m_pPointLight->AddPosition(Vec3(0.f, 0.1f, 0.f));

	if (INPUT->GetButton(I)) // Z+ (앞으로)
		m_pPointLight->AddPosition(Vec3(0.f, 0.f, 0.1f));
	if (INPUT->GetButton(K)) // Z- (뒤로)
		m_pPointLight->AddPosition(Vec3(0.f, 0.f, -0.1f));

	//if (INPUT->GetButton(Z)) // 위쪽 (+Y)
	//	m_pLight->GetLightComponent()->SetDirection(Vec3(0.f, 1.f, 0.f));
	//if (INPUT->GetButton(X)) // 아래쪽 (-Y)
	//	m_pLight->GetLightComponent()->SetDirection(Vec3(0.f, -1.f, 0.f));
	//if (INPUT->GetButton(C)) // 왼쪽 (-X)
	//	m_pLight->GetLightComponent()->SetDirection(Vec3(-1.f, 0.f, 0.f));
	//if (INPUT->GetButton(B)) // 오른쪽 (+X)
	//	m_pLight->GetLightComponent()->SetDirection(Vec3(1.f, 0.f, 0.f));
	//if (INPUT->GetButton(N)) // 앞으로 (-Z)
	//	m_pLight->GetLightComponent()->SetDirection(Vec3(0.f, 0.f, -1.f));
	//if (INPUT->GetButton(M)) // 뒤로 (+Z)
	//	m_pLight->GetLightComponent()->SetDirection(Vec3(0.f, 0.f, 1.f));

	//if (INPUT->GetButtonDown(B))
	//{
	//	m_fBlurScale += 0.5f;
	//	m_fBlurScale = min(m_fBlurScale, 5.0f); // 최대 제한
	//}
	//if (INPUT->GetButtonDown(N))
	//{
	//	m_fBlurScale -= 0.5f;
	//	m_fBlurScale = max(m_fBlurScale, 0.5f); // 최소 제한
	//}

	float deltaTime = TIMER->GetDeltaTime(); // 프레임 보정

	if (m_bIncreasingBlur)
	{
		m_fBlurScale += m_fBlurSpeed * deltaTime;
		if (m_fBlurScale >= 5.0f)
		{
			m_fBlurScale = 5.0f;
			m_bIncreasingBlur = false;
		}
	}
	else
	{
		m_fBlurScale -= m_fBlurSpeed * deltaTime;
		if (m_fBlurScale <= 0.5f)
		{
			m_fBlurScale = 0.5f;
			m_bIncreasingBlur = true;
		}
	}

	POSTPROCESS->SetBlurScale(m_fBlurScale);

	//LIGHTMANAGER->UpdateLightCB();

	
}

void TestSJ::Render()
{
	//DXWRITE->DrawGlow
	//(
	//	D2D1::RectF(300, 300, 600, 400),
	//	L"빛나는 텍스트",
	//	D2D1::ColorF(0.1f, 1.0f, 1.0f, 0.8f), // Glow color (청록빛)
	//	D2D1::ColorF::White                   // 메인 텍스트 색
	//);
}

void TestSJ::SetupObjectEditorCallback()
{
	GUI->SetObjectEditorCallback([this](const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale, Vec3 specularColor, float shininess, Vec3 emissiveColor, float emissivePower, ShapeComponentData shapeData)
		{
			AssimpLoader loader;
			vector<MeshData> meshList = loader.Load(objPath);
			if (meshList.empty())
				return;

			auto meshComp = make_shared<UStaticMeshComponent>();
			meshComp->SetMeshPath(to_mw(objPath));

			auto meshRes = make_shared<UStaticMeshResources>();
			meshRes->SetVertexList(meshList[0].m_vVertexList);
			meshRes->SetIndexList(meshList[0].m_vIndexList);
			meshRes->Create();

			meshComp->SetMesh(meshRes);

			auto mat = make_shared<UMaterial>();
			mat->Load(
				std::wstring(texPath, texPath + strlen(texPath)),
				std::wstring(shaderPath, shaderPath + strlen(shaderPath))
			);
			mat->SetEmissiveParams(emissiveColor, emissivePower);
			mat->SetSpecularParams(specularColor, shininess);
			meshComp->SetMaterial(mat);
			

			// Snap 적용 여부 확인
			if (GUI->GetObjectEditorUI()->IsSnapEnabled())
			{
				pos = GUI->GetObjectEditorUI()->SnapToGrid(pos, 10.0f);
			}

			auto actor = make_shared<APawn>();
			actor->m_szName = L"Object";

			actor->SetMeshComponent(meshComp);
			actor->SetPosition(pos);
			actor->SetRotation(rot);
			actor->SetScale(scale);

			OBJECT->AddActor(actor);
		});
}

void TestSJ::LoadAllPrefabs(const std::string& extension)
{
	auto files = PREFAB->GetPrefabFileList("../Resources/Prefab/", extension);

	for (const auto& file : files)
	{
		if (extension == ".map.json")
		{
			PrefabMapData mapData;
			if (PREFAB->LoadMapTile(file, mapData))
			{
				auto tile = std::make_shared<ATerrainTileActor>();
				tile->m_szName = L"Terrain";
				tile->m_iNumCols = mapData.Cols;
				tile->m_iNumRows = mapData.Rows;
				tile->m_fCellSize = mapData.CellSize;
				tile->CreateTerrain(to_mw(mapData.TexturePath), to_mw(mapData.ShaderPath));
				tile->SetPosition(mapData.Position);
				tile->SetRotation(mapData.Rotation);
				tile->SetScale(mapData.Scale);
				OBJECT->AddActor(tile);
			}
		}
		else if (extension == ".character.json")
		{
			PrefabCharacterData characterData;
			if (PREFAB->LoadCharacter(file, characterData))
			{
				auto actor = std::make_shared<AActor>(); // 필요에 따라 캐릭터 타입으로 변경

				shared_ptr<UMeshComponent> meshComponent = meshLoader.Make(characterData.MeshPath.c_str());

				actor->SetMeshComponent(meshComponent);

				actor->m_szName = L"Character";
				actor->SetPosition(Vec3(characterData.transform.Position));
				actor->SetRotation(Vec3(characterData.transform.Rotation));
				actor->SetScale(Vec3(characterData.transform.Scale));

				if (characterData.ScriptType == 1) actor->AddScript(std::make_shared<PlayerMoveScript>());

				if (characterData.camera.isUse)
				{
					auto cameraComponent = make_shared<UCameraComponent>();
					cameraComponent->SetLocalPosition(Vec3(characterData.camera.Position));
					cameraComponent->SetLocalRotation(Vec3(characterData.camera.Rotation));
					cameraComponent->SetPerspective(characterData.camera.Fov, characterData.camera.Aspect, characterData.camera.Near, characterData.camera.Far);
					actor->SetCameraComponent(cameraComponent);
				}

				if (characterData.shape.isUse)
				{
					shared_ptr<UShapeComponent> shapeComponent = nullptr;
					if (static_cast<ShapeType>(characterData.shape.eShapeType) == ShapeType::ST_BOX)
						shapeComponent = make_shared<UBoxComponent>();
					//else if (static_cast<ShapeType>(characterData.shape.eShapeType) == ShapeType::ST_SPHERE)

					shapeComponent->SetLocalScale(Vec3(characterData.shape.Scale));
					shapeComponent->SetLocalPosition(Vec3(characterData.shape.Position));
					shapeComponent->SetLocalRotation(Vec3(characterData.shape.Rotation));
					shapeComponent->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
					actor->SetShapeComponent(shapeComponent);
				}


				OBJECT->AddActor(actor);
			}
		}
		else if (extension == ".object.json")
		{
			PrefabObjectData objData;
			if (PREFAB->LoadObject(file, objData))
			{
				shared_ptr<UStaticMeshComponent> meshComp = make_shared<UStaticMeshComponent>();
				if (SplitExt(to_mw(objData.MeshPath)) == L".obj")
				{
					AssimpLoader loader;
					vector<MeshData> meshList = loader.Load(objData.MeshPath.c_str());
					meshComp->SetMeshPath(to_mw(objData.MeshPath));
					auto meshRes = make_shared<UStaticMeshResources>();
					if (!meshList.empty())
					{
						meshRes->SetVertexList(meshList[0].m_vVertexList);
						meshRes->SetIndexList(meshList[0].m_vIndexList);
						meshRes->Create();
						meshComp->SetMesh(meshRes);
					}

				}
				else
				{
					ActorLoader al;
					al.LoadOne(objData.MeshPath);
					meshComp->SetMeshPath(to_mw(objData.MeshPath));
					auto resources = al.LoadMeshResources();
					meshComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(resources[0]));
				}


				auto material = make_shared<UMaterial>();
				material->Load(to_mw(objData.TexturePath), to_mw(objData.ShaderPath));
				meshComp->SetMaterial(material);

				auto obj = make_shared<APawn>();
				obj->m_szName = L"Object";
				obj->SetMeshComponent(meshComp);
				obj->SetPosition(objData.Position);
				obj->SetRotation(objData.Rotation);
				obj->SetScale(objData.Scale);

				OBJECT->AddActor(obj);
			}
		}
	}
}