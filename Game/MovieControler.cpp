#include "pch.h"
#include "MovieControler.h"

// Manager
#include "Timer.h"

// Actor
#include "ACameraActor.h"

MovieControler::MovieControler()
{
	m_pMovieCamera = make_shared<ACameraActor>();
}

void MovieControler::SetCameraStart(Vec3 _pos, Vec3 _rot)
{
	m_pMovieCamera->SetPosition(_pos);
	m_pMovieCamera->SetRotation(_rot);
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

	// 예린이 필요한거
	// 1. Betty의 등장 연출 시작
	// 2. 함수 종료

	static float time = 0.f;
	time += TIMER->GetDeltaTime();

	if (time >= 0.f && time < m_fTrack1)
	{
		// Fence가 쳐지며 까마귀가 시작위치로 가는 구간


	}
	else if (time >= m_fTrack1 && time < m_fTrack2)
	{
		// 까마귀가 카메라 안으로 들어와서 멈추는 구간

		// 1 Frame true용
		if (m_bStartTrack2)
			m_bStartTrack2 = false;
		m_bStartTrack2 = true;

		// Camera의 움직임


	}
	else if (time >= m_fTrack2 && time < m_fTrack3)
	{
		// 베티의 등장 연출(애니메이션)을 보여주는 구간

		// 1 Frame true용
		if (m_bStartTrack3)
			m_bStartTrack3 = false;
		m_bStartTrack3 = true;


	}
	else if (time >= m_fTrack3 && time < m_fTrack4)
	{
		// 잠깐의 시간 준다

		// 1 Frame true용
		if (m_bStartTrack4)
			m_bStartTrack4 = false;
		m_bStartTrack4 = true;

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