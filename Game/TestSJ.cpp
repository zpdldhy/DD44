#include "pch.h"
#include "TestSJ.h"
#include "Input.h"
#include "DxWrite.h"
#include "Sound.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "Timer.h"

void TestSJ::Init()
{
	SOUNDMANAGER->LoadAllSounds();


	m_pStaticMesh = make_shared<UStaticMeshComponent>();
	m_pStaticMesh->CreateCube();
	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Glow.hlsl");
	m_pStaticMesh->SetMaterial(material);
}

void TestSJ::Update()
{
	{
		static bool bGlow = false;
		static float glowTimer = 0.0f;

		// R 키를 누르면 발광 시작
		if (INPUT->GetButtonDown(R))
		{
			bGlow = true;
			glowTimer = 0.1f; // 1초 동안 발광
		}

		// 발광 중이면 타이머 감소
		if (bGlow)
		{
			glowTimer -= TIMER->GetDeltaTime();
			if (glowTimer <= 0.0f)
			{
				bGlow = false;
				glowTimer = 0.0f;
			}
		}

		// 현재 발광 값 계산
		float glowPower = bGlow ? 2.0f : 0.0f;

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
	m_pStaticMesh->Render();
}
