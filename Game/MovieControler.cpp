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

	// ������ �ʿ��Ѱ�
	// 1. Betty�� ���� ���� ����
	// 2. �Լ� ����

	static float time = 0.f;
	time += TIMER->GetDeltaTime();

	if (time >= 0.f && time < m_fTrack1)
	{
		// Fence�� ������ ��Ͱ� ������ġ�� ���� ����


	}
	else if (time >= m_fTrack1 && time < m_fTrack2)
	{
		// ��Ͱ� ī�޶� ������ ���ͼ� ���ߴ� ����

		// 1 Frame true��
		if (m_bStartTrack2)
			m_bStartTrack2 = false;
		m_bStartTrack2 = true;

		// Camera�� ������


	}
	else if (time >= m_fTrack2 && time < m_fTrack3)
	{
		// ��Ƽ�� ���� ����(�ִϸ��̼�)�� �����ִ� ����

		// 1 Frame true��
		if (m_bStartTrack3)
			m_bStartTrack3 = false;
		m_bStartTrack3 = true;


	}
	else if (time >= m_fTrack3 && time < m_fTrack4)
	{
		// ����� �ð� �ش�

		// 1 Frame true��
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