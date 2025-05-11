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

void TestSJ::Init()
{
	SOUNDMANAGER->LoadAllSounds();


	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}

	{
		m_pActor = make_shared<APawn>();

		m_pStaticMesh = UStaticMeshComponent::CreateSphere(20, 20);
		m_pActor->SetMeshComponent(m_pStaticMesh);
		m_pActor->SetScale({ 5.0f, 5.0f, 5.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 10.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Effect.hlsl");
		m_pStaticMesh->SetMaterial(material);
	}

	{
		m_pActor2 = make_shared<APawn>();

		m_pStaticMesh2 = UStaticMeshComponent::CreateSphere(20, 20);


		m_pActor2->SetMeshComponent(m_pStaticMesh2);
		m_pActor2->SetScale({ 1.0f, 1.0f, 1.0f });
		m_pActor2->SetPosition({ 5.0f, 0.0f, 10.0f }); // ù ��° ť�� ���� ��ġ
		m_pActor2->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material2 = make_shared<UMaterial>();
		material2->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Effect.hlsl");
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

	{
		auto noiseTex = TEXTURE->Load(L"../Resources/Texture/Noise.png");
		m_pStaticMesh->GetMaterial()->SetNoiseTexture(noiseTex);
	}

	m_pLight = make_shared<ALight>();


	CAMERAMANAGER->SetCameraActor(m_pCameraActor);

	GUI->SetEffectEditorCallback(
		[this](int selected, float glowPower, Vec3 glowColor, float dissolveThreshold, Vec3 emissiveColor, float emissivePower) {
			if (selected == 0 && m_pStaticMesh)
			{
				targetMat = m_pStaticMesh->GetMaterial();
			}
			else if (selected == 1 && m_pStaticMesh2)
			{
				targetMat = m_pStaticMesh2->GetMaterial();
			}
			else if (selected == 2)
			{
				targetMat = m_vMeshList[2]->GetMaterial();
			}

			if (targetMat)
			{
				targetMat->SetGlowParams(glowPower, glowColor);
				targetMat->SetDissolveParams(dissolveThreshold);
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

	// �޽� �Ľ�
	{
		m_pLoader = make_shared<ActorLoader>();
		m_pLoader->LoadOne("../Resources/Asset/crow_final.asset");
		m_vMeshList = m_pLoader->LoadMesh();
		// 2�� �ε����� ��. meshComponent Ÿ��Ÿ�� UObject�� �̸� Ȯ���غ���, "detailSword_weaponTexuture1".
		m_pSwordActor = make_shared<APawn>();
		m_pSwordActor->SetMeshComponent(m_vMeshList[2]);
		m_pSwordActor->SetPosition(Vec3(10.f, 0.0f, 25.0f));
		m_pSwordActor->SetScale(Vec3(10.0f, 10.0f, 10.0f));
		m_vMeshList[2]->GetMaterial()->SetUseBloom(true);
	}


	OBJECTMANAGER->AddActor(m_pLight);
	OBJECTMANAGER->AddActor(m_pCameraActor);
	OBJECTMANAGER->AddActor(m_pActor);
	OBJECTMANAGER->AddActor(m_pActor2);
	OBJECTMANAGER->AddActor(m_pSky);
	OBJECTMANAGER->AddActor(m_pSwordActor);
}

void TestSJ::Update()
{
	// ������Ʈ ȸ��
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
	if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
	{
		Vec3 camPos = m_pCameraActor->GetCameraComponent()->GetCameraPos();
		m_pStaticMesh->GetMaterial()->SetCameraPos(camPos);
		m_pStaticMesh2->GetMaterial()->SetCameraPos(camPos);
		m_vMeshList[2]->GetMaterial()->SetCameraPos(camPos);
	}
	//Emissive
	{
		m_pStaticMesh2->GetMaterial()->SetEmissiveParams(Vec3(0.0f, 1.0f, 0.0f), 0.1f);
	}
	//UVDistortion
	{
		static bool bUVInitialized = false;
		if (!bUVInitialized)
		{
			m_pStaticMesh->GetMaterial()->SetUVDistortionParams(0.08f, 1.0f, 15.0f);
			m_pStaticMesh2->GetMaterial()->SetUVDistortionParams(0.01f, 1.5f, 8.0f);
			bUVInitialized = true;
		}

		float delta = TIMER->GetDeltaTime();

		if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
		{
			auto material = m_pStaticMesh->GetMaterial();
			material->m_tEffectData.g_vDistortion.w += delta * material->m_tEffectData.g_vDistortion.y; // Time += dt * speed
			material->m_tEffectData.g_vDistortion.w = fmodf(material->m_tEffectData.g_vDistortion.w, DD_PI * 2);
		}
		if (m_pStaticMesh2 && m_pStaticMesh2->GetMaterial())
		{
			auto material = m_pStaticMesh2->GetMaterial();
			material->m_tEffectData.g_vDistortion.w += delta * material->m_tEffectData.g_vDistortion.y;
			material->m_tEffectData.g_vDistortion.w = fmodf(material->m_tEffectData.g_vDistortion.w, DD_PI * 2);
		}
	}


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

	// fallback ���� ���� (������ ��Ȯ�ϴϱ�)
	//GUI->SetTargetMaterial(targetMat);
	//Glow
	{
		//static bool bGlow = false;
		//static float glowTimer = 0.0f;
		//const float maxGlowDuration = 0.5f;

		//// R Ű�� ������ �߱� ����
		//if (INPUT->GetButtonDown(R))
		//{
		//	bGlow = true;
		//	glowTimer = maxGlowDuration;
		//}

		//float glowPower = 0.0f;

		//if (bGlow)
		//{
		//	glowTimer -= TIMER->GetDeltaTime();

		//	float ratio = glowTimer / maxGlowDuration; // 1.0 �� 0.0 
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

		//if (INPUT->GetButtonDown(R)) // �ǰ� ����
		//{
		//	flashTimer = 1.0f; // Flash ����
		//}

		//flashTimer -= TIMER->GetDeltaTime() * 3.0f; // ������ �������
		//flashTimer = max(flashTimer, 0.0f);

		//if (targetMat)
		//{
		//	targetMat->SetHitFlashTime(flashTimer);
		//}
		static float flashTimer = 0.0f;

		if (INPUT->GetButtonDown(R))
			flashTimer = 1.0f;

		flashTimer -= TIMER->GetDeltaTime() * 3.0f;
		flashTimer = max(flashTimer, 0.0f);

		int selected = GUI->GetEffectEditorUI()->GetSelectedActor(); // �Ǵ� ���� selected �� ��������

		if (selected == 0 && m_pStaticMesh)
			m_pStaticMesh->GetMaterial()->SetHitFlashTime(flashTimer);
		else if (selected == 1 && m_pStaticMesh2)
			m_pStaticMesh2->GetMaterial()->SetHitFlashTime(flashTimer);
		else if (selected == 2)
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
		DXWRITE->SetFont(L"�Ҿ߶�9");
	}

	if (INPUT->GetButton(S))
	{
		DXWRITE->SetFont(L"�ü�");
	}

	if (INPUT->GetButton(D))
	{
		DXWRITE->SetFont(L"Rubik-Beastly");
	}

	if (INPUT->GetButton(W))
	{
		DXWRITE->SetFont(L"Mapoȫ��������");
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
}

void TestSJ::Render()
{
	//DXWRITE->DrawGlow
	//(
	//	D2D1::RectF(300, 300, 600, 400),
	//	L"������ �ؽ�Ʈ",
	//	D2D1::ColorF(0.1f, 1.0f, 1.0f, 0.8f), // Glow color (û�Ϻ�)
	//	D2D1::ColorF::White                   // ���� �ؽ�Ʈ ��
	//);
}
