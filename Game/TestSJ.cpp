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

		m_pStaticMesh = UStaticMeshComponent::CreateCube();
		m_pActor->SetMesh(m_pStaticMesh);
		m_pActor->SetScale({ 1.0f, 1.0f, 1.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 10.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Dissolve.hlsl");
		m_pStaticMesh->SetMaterial(material);
	}

	{
		m_pSky = make_shared<ASky>();

		m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
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
	m_pSky->Init();
}

void TestSJ::Update()
{
	//Glow
	{
		static bool bGlow = false;
		static float glowTimer = 0.0f;
		const float maxGlowDuration = 0.5f;

		// R Å°¸¦ ´©¸£¸é ¹ß±¤ ½ÃÀÛ
		if (INPUT->GetButtonDown(R))
		{
			bGlow = true;
			glowTimer = maxGlowDuration;
		}

		float glowPower = 0.0f;

		if (bGlow)
		{
			glowTimer -= TIMER->GetDeltaTime();

			float ratio = glowTimer / maxGlowDuration; // 1.0 ¡æ 0.0 
			glowPower = 1000000.0f;//ratio*5.0f;
			
			if (glowTimer <= 0.0f)
			{
				bGlow = false;
				glowTimer = 0.0f;
			}
		}

		if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
		{
			m_pStaticMesh->GetMaterial()->SetGlowParams(
				GUI->GetGlowPower(),//glowPower,
				GUI->GetGlowColor()
			);
		}

	}
	//Dissolve
	{
		if (m_pStaticMesh && m_pStaticMesh->GetMaterial())
		{
			m_pStaticMesh->GetMaterial()->SetDissolveParams(GUI->GetDissolveThreshold());
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
		DXWRITE->SetFont(L"¼Ò¾ß¶ã9");
	}

	if (INPUT->GetButton(S))
	{
		DXWRITE->SetFont(L"±Ã¼­");
	}

	if (INPUT->GetButton(D))
	{
		DXWRITE->SetFont(L"Rubik-Beastly");
	}

	if (INPUT->GetButton(W))
	{
		DXWRITE->SetFont(L"MapoÈ«´ëÇÁ¸®´ý");
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
		m_pSky->Tick();
	}

}

void TestSJ::Render()
{
	DXWRITE->DrawGlow
	(
		D2D1::RectF(300, 300, 600, 400),
		L"ºû³ª´Â ÅØ½ºÆ®",
		D2D1::ColorF(0.1f, 1.0f, 1.0f, 0.8f), // Glow color (Ã»·Ïºû)
		D2D1::ColorF::White                   // ¸ÞÀÎ ÅØ½ºÆ® »ö
	);
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pSky->Render();
}
