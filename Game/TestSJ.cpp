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

void TestSJ::Init()
{
	SOUNDMANAGER->LoadAllSounds();


	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 10.0f });
	}

	{
		m_pActor = make_shared<APawn>();

		m_pStaticMesh = make_shared<UStaticMeshComponent>();
		m_pStaticMesh->CreateCube();
		m_pActor->SetMesh(m_pStaticMesh);
		m_pActor->SetScale({ 1.0f, 1.0f, 1.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Glow.hlsl");
		m_pStaticMesh->SetMaterial(material);
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

	m_pCameraActor->Init();
	m_pActor->Init();
	m_pSky->Init();
}

void TestSJ::Update()
{
	{
		static bool bGlow = false;
		static float glowTimer = 0.0f;
		const float maxGlowDuration = 0.5f;

		// R 키를 누르면 발광 시작
		if (INPUT->GetButtonDown(R))
		{
			bGlow = true;
			glowTimer = maxGlowDuration;
		}

		//// 발광 중이면 타이머 감소
		//if (bGlow)
		//{
		//	glowTimer -= TIMER->GetDeltaTime();
		//	if (glowTimer <= 0.0f)
		//	{
		//		bGlow = false;
		//		glowTimer = 0.0f;
		//	}
		//}

		//// 현재 발광 값 계산
		//float glowPower = bGlow ? 2.0f : 0.0f;
		float glowPower = 0.0f;

		if (bGlow)
		{
			glowTimer -= TIMER->GetDeltaTime();

			float ratio = glowTimer / maxGlowDuration; // 1.0 → 0.0
			//glowPower = ratio * 3.0f;         
			glowPower = sin(ratio * DD_PI);
			
			if (glowTimer <= 0.0f)
			{
				bGlow = false;
				glowTimer = 0.0f;
			}
		}

		// Material에 업데이트
		m_pStaticMesh->GetMaterial()->SetGlowParams(
			glowPower,
			Vec3(1.0f, 0.0f, 0.0f) // 빨간색
		);

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
	//SKY
	{
		float deltaTime = TIMER->GetDeltaTime() * 10.f;

		if (INPUT->GetButtonDown(W))
		{
			m_pCameraActor->AddPosition(Vec3(0.0f, 0.0f, -1.0f) * deltaTime);
		}
		if (INPUT->GetButtonDown(A))
		{
			m_pCameraActor->AddPosition(Vec3(-1.0f, 0.0f, 0.0f) * deltaTime);
		}
		if (INPUT->GetButtonDown(S))
		{
			m_pCameraActor->AddPosition(Vec3(0.0f, 0.0f, 1.0f) * deltaTime);
		}
		if (INPUT->GetButtonDown(D))
		{
			m_pCameraActor->AddPosition(Vec3(1.0f, 0.0f, 0.0f) * deltaTime);
		}
		if (INPUT->GetButtonDown(Q))
		{
			m_pCameraActor->AddPosition(Vec3(0.0f, -1.0f, 0.0f) * deltaTime);
		}
		if (INPUT->GetButtonDown(E))
		{
			m_pCameraActor->AddPosition(Vec3(0.0f, 1.0f, 0.0f) * deltaTime);
		}


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
		L"빛나는 텍스트",
		D2D1::ColorF(0.1f, 1.0f, 1.0f, 0.8f), // Glow color (청록빛)
		D2D1::ColorF::White                   // 메인 텍스트 색
	);
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pSky->Render();
}
