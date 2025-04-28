#include "pch.h"
#include "Sound.h"


void Sound::Update()
{
	if (m_pSystem)
	{
		m_pSystem->update();
	}
}

void Sound::Destroy()
{
	if (m_pSound)
	{
		m_pSound->release();
		m_pSound = nullptr;
	}
}


bool Sound::Load(FMOD::System* _pSystem, std::wstring _filename)
{
	m_pSystem = _pSystem;
	FMOD_RESULT hr = m_pSystem->createSound(to_wm(_filename).c_str(), FMOD_DEFAULT, 0, &m_pSound);
	if (hr == FMOD_OK)
	{
		return true;
	}
	return false;
}

void	Sound::Play2D(bool bLoop)
{
	bool  bPlay = false;
	if (m_pChannel != nullptr)
	{
		m_pChannel->isPlaying(&bPlay);
	}
	if (bPlay == false)
	{
		FMOD_RESULT hr = m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
		if (hr == FMOD_OK)
		{
			m_pChannel->setVolume(m_fVolume);
			if (bLoop)
				m_pChannel->setMode(FMOD_LOOP_NORMAL);
			else
				m_pChannel->setMode(FMOD_LOOP_OFF);
		}

	}
}
void	Sound::PlayEffect2D(bool bLoop)
{
	FMOD_RESULT hr = m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
	if (hr == FMOD_OK)
	{
		if (hr == FMOD_OK)
		{
			m_pChannel->setVolume(m_fVolume);
			if (bLoop)
				m_pChannel->setMode(FMOD_LOOP_NORMAL);
			else
				m_pChannel->setMode(FMOD_LOOP_OFF);
		}
	}
}

void Sound::Stop()
{
	if (m_pChannel)
	{
		m_pChannel->stop();
		m_pChannel = nullptr;
	}
}

void Sound::Paused()
{
	bool bPlay = false;
	m_pChannel->isPlaying(&bPlay);
	if (bPlay)
	{
		bool paused;
		m_pChannel->getPaused(&paused);
		m_pChannel->setPaused(!paused);
	}
}

void Sound::VolumeUp(float fVolume)
{
	if (m_pChannel)
	{
		float fCurrentVolume;
		m_pChannel->getVolume(&fCurrentVolume);
		m_fVolume = fCurrentVolume + fVolume;
		m_fVolume = max(0.0f, m_fVolume);
		m_fVolume = min(1.0f, m_fVolume);
		m_pChannel->setVolume(m_fVolume);
	}
}

void Sound::VolumeDown(float fVolume)
{
	if (m_pChannel)
	{
		float fCurrentVolume;
		m_pChannel->getVolume(&fCurrentVolume);
		m_fVolume = fCurrentVolume - fVolume;
		m_fVolume = max(0.0f, m_fVolume);
		m_fVolume = min(1.0f, m_fVolume);
		m_pChannel->setVolume(m_fVolume);
	}
}

Sound* SoundManager::Load(std::wstring filename)
{
	if (m_pSystem == nullptr)
	{
		FMOD_RESULT hr = FMOD::System_Create(
			&m_pSystem);
		if (hr == FMOD_OK)
		{
			m_pSystem->init(32, FMOD_INIT_NORMAL, 0);
		}
	}

	auto key = SplitPath(filename);
	auto data = GetPtr(key);
	if (data != nullptr)
	{
		return data; // 이미 로딩된 사운드가 있으면 리턴
	}

	Sound* pSound = new Sound(key); // 새 Sound 객체 생성

	if (pSound->Load(m_pSystem, filename))
	{
		maplist.insert(std::make_pair(key, pSound));
		return pSound; //  새로 로드한 사운드를 리턴
	}
	else
	{
		delete pSound; //  로드 실패했으면 메모리 해제
		return nullptr;
	}
}

Sound* SoundManager::GetPtr(std::wstring key)
{
	auto data = maplist.find(key);
	if (data != maplist.end())
	{
		return data->second;
	}
	return nullptr;
}

void SoundManager::Update()
{
	for (auto sound : maplist)
	{
		sound.second->Update();
	}
}


SoundManager::~SoundManager()
{
	for (auto sound : maplist)
	{
		sound.second->Destroy();
		delete sound.second;
	}
	maplist.clear();
	if (m_pSystem)
	{
		m_pSystem->close();
		m_pSystem->release();
	}
}
