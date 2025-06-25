#include "pch.h"
#include "MovieControler.h"

// Manager
#include "Timer.h"
#include "CameraManager.h"
#include "ObjectManager.h"

// Actor
#include "ACameraActor.h"

void MovieControler::SetCameraStart(Vec3 _pos, Vec3 _rot)
{
	m_pMovieCamera = make_shared<ACameraActor>();
	m_pMovieCamera->m_szName = L"MovieCamera";
	m_pMovieCamera->SetPosition(_pos);
	m_pMovieCamera->SetRotation(_rot);
	m_pMovieCamera->GetCameraComponent()->SetLookTo(m_pMovieCamera->GetLook());
	OBJECT->AddActor(m_pMovieCamera);
}

void BettyMeetControler::init()
{
}

void BettyMeetControler::Tick()
{
	if (!m_bStartMovie) 
	{
		m_bEndMovie = false;
		return; 
	}

	ResetData();

	// ������ �ʿ��Ѱ�
	// 1. Betty�� ���� ���� ����
	// 2. �Լ� ����

	static float time = 0.f;
	time += TIMER->GetDeltaTime();

	if (time < m_fTrack1)
	{
		static bool bFirstFrame = true;
		if (bFirstFrame)
			m_bStartTrack1 = true;
		else
		{
			m_bStartTrack1 = false;
			m_bMoveEnd = false;
		}

		// Fence�� ������ ��Ͱ� ������ġ�� ���� ����
		float track1 = time / m_fMoveTime1;
		if (time < m_fMoveTime1)
		{
			m_vPlayerCurrentPos = LerpEased(m_vPlayerEnterPos, m_vPlayerStartPos, track1);
			auto enterLook = m_vPlayerStartPos - m_vPlayerEnterPos;	// Look
			m_vPlayerCurrentRot = Vec3(asinf(-enterLook.y), atan2f(enterLook.x, enterLook.z), 0);
		}
		else if (time >= m_fMoveTime1 && time < m_fRotTime1)
		{
			static float startRot = m_vPlayerCurrentRot.y;
			static float tempTime = (m_fRotTime1 - time);
			float turnTime = (time - m_fMoveTime1) / tempTime;	// ȸ�� �ð�
			m_vPlayerCurrentRot.y = Lerp(startRot, -0.9164f, turnTime);
		}

		bFirstFrame = false;
	}
	else if (time >= m_fTrack1 && time < m_fTrack2)
	{
		static bool bFirstFrame = true;
		if (bFirstFrame)
			m_bStartTrack2 = true;
		else
		{
			m_bStartTrack2 = false;
			m_bMoveEnd = false;
		}

		// ��Ͱ� ī�޶� ������ ���ͼ� ���ߴ� ����
		static float tempTime = (m_fMoveTime2 - time);
		float track2 = (time - m_fTrack1) / tempTime;
		if (time < m_fMoveTime2)
		{
			m_vPlayerCurrentPos = LerpEased(m_vPlayerStartPos, m_vPlayerEndPos, track2);
			auto look = m_vPlayerEndPos - m_vPlayerStartPos;	// Look
			m_vPlayerCurrentRot = Vec3(asinf(-look.y), atan2f(look.x, look.z), 0);
		}
		else
		{
			m_bMoveEnd = true;
		}

		// Camera�� ������
		CAMERA->Set3DCameraActor(m_pMovieCamera);

		bFirstFrame = false;
	}
	else if (time >= m_fTrack2 && time < m_fTrack3)
	{
		static bool bFirstFrame = true;
		if (bFirstFrame)
			m_bStartTrack3 = true;
		else
			m_bStartTrack3 = false;

		// ��Ƽ�� ���� ����(�ִϸ��̼�)�� �����ִ� ����


		bFirstFrame = false;
	}
	else if (time >= m_fTrack3 && time < m_fTrack4)
	{
		static bool bFirstFrame = true;
		if (bFirstFrame)
			m_bStartTrack4 = true;
		else
			m_bStartTrack4 = false;

		// ����� �ð� �ش�


		bFirstFrame = false;
	}
	else
	{
		m_bStartMovie = false;
		m_bEndMovie = true;
	}

	if (time > m_fPopUpTime)
		m_bUIPopUp = true;

	if (time > m_fPopDownTime)
		m_bUIPopDown = true;
}

void BettyMeetControler::Destroy()
{
}

void BettyMeetControler::ResetData()
{
	m_bStartTrack2 = false;
	m_bStartTrack3 = false;
	m_bStartTrack4 = false;
	m_bUIPopUp = false;
	m_bUIPopDown = false;
}