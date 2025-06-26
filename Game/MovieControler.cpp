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
		return; 
	}

	ResetData();

	// 예린이 필요한거
	// 1. Betty의 등장 연출 시작
	// 2. 함수 종료

	static float time = 0.f;
	time += TIMER->GetDeltaTime();

	// Track1
	if (time < m_fTrack1)
	{
		static bool bFirstFrame = true;
		static bool checkMoveEnd = false;

		if (bFirstFrame)
			m_bStartTrack1 = true;
		else
		{
			m_bStartTrack1 = false;
		}

		if (m_bMoveEnd == true && checkMoveEnd == false)
		{
			m_bMoveEnd = false;
			checkMoveEnd = true;
		}

		// Fence가 쳐지며 까마귀가 시작위치로 가는 구간
		float track1 = time / m_fMoveTime1;
		if (time < m_fMoveTime1)
		{
			m_vPlayerCurrentPos = LerpEased(m_vPlayerEnterPos, m_vPlayerStartPos, track1);
			auto enterLook = m_vPlayerStartPos - m_vPlayerEnterPos;	// Look
			m_vPlayerCurrentRot = Vec3(asinf(-enterLook.y), atan2f(enterLook.x, enterLook.z), 0);
		}
		// 까마귀가 회전하는 구간
		else if (time >= m_fMoveTime1 && time < m_fRotTime1)
		{
			static float startRot = m_vPlayerCurrentRot.y;
			static float tempTime = (m_fRotTime1 - time);
			float turnTime = (time - m_fMoveTime1) / tempTime;	// 회전 시간
			m_vPlayerCurrentRot.y = Lerp(startRot, -0.9164f, turnTime);
		}
		else if (checkMoveEnd == false)
		{
			m_bMoveEnd = true;
		}

		bFirstFrame = false;
	}

	// Track2
	else if (time >= m_fTrack1 && time < m_fTrack2)
	{
		static bool bFirstFrame = true;
		static bool checkMoveEnd = false;

		if (bFirstFrame)
		{
			// Camera의 전환
			CAMERA->Set3DCameraActor(m_pMovieCamera);
			m_bStartTrack2 = true;
		}
		else
		{
			m_bStartTrack2 = false;
		}

		if (m_bMoveEnd == true && checkMoveEnd == false)
		{
			m_bMoveEnd = false;
			checkMoveEnd = true;
		}

		// 까마귀가 카메라 안으로 들어와서 멈추는 구간
		if (time < m_fMoveTime2)
		{
			static float tempTime = (m_fMoveTime2 - time);
			float move2 = (time - m_fTrack1) / tempTime;

			m_vPlayerCurrentPos = LerpEased(m_vPlayerStartPos, m_vPlayerEndPos, move2);
			auto look = m_vPlayerEndPos - m_vPlayerStartPos;	// Look
			m_vPlayerCurrentRot = Vec3(asinf(-look.y), atan2f(look.x, look.z), 0);
		}
		else if (checkMoveEnd == false)
		{
			m_bMoveEnd = true;
		}
		// 잠깐 움직임을 멈추고 쉬는 구간
		else if (time < m_fRestTime)
		{

		}
		// 베티까지 Camera Close Up
		else if (time >= m_fRestTime && time < m_fTrack2)
		{
			static Vec3 vCurrentCameraPos = m_pMovieCamera->GetPosition();
			static float tempTime = (m_fTrack2 - time);
			float track2 = (time - m_fRestTime) / tempTime;

			m_pMovieCamera->SetPosition(LerpEased(vCurrentCameraPos, Vec3(49.5f, 6.5f, -83.f), track2));

			static float startRot = m_pMovieCamera->GetRotation().x;
			auto tempRot = Lerp(startRot, 0.05f, track2);
			auto currentRot = m_pMovieCamera->GetRotation();
			currentRot.x = tempRot;
			m_pMovieCamera->SetRotation(currentRot);
		}

		bFirstFrame = false;
	}

	// Track3
	else if (time >= m_fTrack2 && time < m_fTrack3)
	{
		static bool bFirstFrame = true;
		if (bFirstFrame)
			m_bStartTrack3 = true;
		else
			m_bStartTrack3 = false;

		// 베티의 등장 연출(애니메이션)을 보여주는 구간
		if (time > m_fPopUpTime)
			m_bUIPopUp = true;

		bFirstFrame = false;
	}

	else
	{
		m_bStartMovie = false;
		m_bEndMovie = true;
	}
}

void BettyMeetControler::Destroy()
{
}

void BettyMeetControler::ResetData()
{
	m_bStartTrack2 = false;
	m_bStartTrack3 = false;
	m_bUIPopUp = false;
}