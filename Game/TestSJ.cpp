#include "pch.h"
#include "TestSJ.h"
#include "Input.h"
#include "DxWrite.h"
#include "Sound.h"
void TestSJ::Init()
{
	m_pBomb = SOUNDMANAGER->Load(L"../Resources/Sound/bomb.wav");
	//m_pBgm = SOUNDMANAGER->Load(L"../Resources/Sound/Boss.mp3");
}

void TestSJ::Update()
{
	//m_pBgm->Play2D();
	if (INPUT->GetButton(A))
	{
		m_pBomb->PlayEffect2D();
	}

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

void TestSJ::Render()
{


	//DXWRITE->DrawGlow(
	//	D2D1::RectF(300, 300, 600, 400),
	//	L"빛나는 텍스트",
	//	D2D1::ColorF(0.1f, 1.0f, 1.0f, 0.8f), // Glow color (청록빛)
	//	D2D1::ColorF::White                   // 메인 텍스트 색
	//);
}
