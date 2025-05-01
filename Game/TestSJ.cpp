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

		m_pStaticMesh = make_shared<UStaticMeshComponent>();
		m_pStaticMesh->CreateCube();
		m_pActor->SetMesh(m_pStaticMesh);
		m_pActor->SetScale({ 1.0f, 1.0f, 1.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 10.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/uvDistortion.hlsl");
		m_pStaticMesh->SetMaterial(material);
	}

	{
		m_pActor2 = make_shared<APawn>();

		m_pStaticMesh2 = make_shared<UStaticMeshComponent>();
		m_pStaticMesh2->CreateCube();

		m_pActor2->SetMesh(m_pStaticMesh2);
		m_pActor2->SetScale({ 1.0f, 1.0f, 1.0f });
		m_pActor2->SetPosition({ 5.0f, 0.0f, 10.0f }); // ù ��° ť�� ���� ��ġ
		m_pActor2->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material2 = make_shared<UMaterial>();
		material2->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Dissolve.hlsl");
		m_pStaticMesh2->SetMaterial(material2);
	}

	{
		m_pSky = make_shared<ASky>();

		m_pSkyMesh = make_shared<UStaticMeshComponent>();
		m_pSkyMesh->CreateSphere(20, 20);
		m_pSky->SetMesh(m_pSkyMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
		m_pSkyMesh->SetMaterial(material);
	}

	{
		auto noiseTex = TEXTURE->Load(L"../Resources/Texture/Noise.png");
		m_pStaticMesh->GetMaterial()->SetNoiseTexture(noiseTex);
	}

	m_pCameraActor->Init();
	m_pActor->Init();
	m_pActor2->Init();
	m_pSky->Init();
}

void TestSJ::Update()
{
	//UVDistortion
	{
		static bool bUVInitialized = false;
		if (!bUVInitialized)
		{
			m_pStaticMesh->GetMaterial()->SetUVDistortionParams(0.08f, 10.0f, 15.0f); // strength, speed, frequency
			m_pStaticMesh2->GetMaterial()->SetUVDistortionParams(0.01f, 1.5f, 8.0f);  // optional: �ٸ� ȿ��
			bUVInitialized = true;
		}

		float deltaTime = TIMER->GetDeltaTime();

		if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
		{
			m_pStaticMesh->GetMaterial()->UpdateUVDistortionBuffer(deltaTime);
		}
		if (m_pStaticMesh2 && m_pStaticMesh2->GetMaterial())
		{
			m_pStaticMesh2->GetMaterial()->UpdateUVDistortionBuffer(deltaTime);
		}
	}


	shared_ptr<UMaterial> targetMat = nullptr;
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
	}

	
	if (GUI->m_iSelectedActor == 0 && m_pStaticMesh)
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
		static float flashTimer = 0.0f;

		if (INPUT->GetButtonDown(R)) // �ǰ� ����
		{
			flashTimer = 1.0f; // Flash ����
		}

		flashTimer -= TIMER->GetDeltaTime() * 3.0f; // ������ �������
		flashTimer = max(flashTimer, 0.0f);

		if (targetMat)
		{
			targetMat->SetHitFlashTime(flashTimer);
		}
	}
	//Sound
	{
		//m_pBgm->Play2D();
		if (INPUT->GetButton(A))
		{
			SOUNDMANAGER->GetPtr(ESoundType::Bomb)->PlayEffect2D();
		}
	}
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
	//Camera
	{
		m_pCameraActor->Tick();
		m_pActor->Tick();
		m_pActor2->Tick();
		m_pSky->Tick();
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
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pActor2->Render();
	m_pSky->Render();
}
