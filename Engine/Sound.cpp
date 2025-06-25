#include "pch.h"
#include "Sound.h"

void Sound::Update()
{
	if (m_pSystem)
		m_pSystem->update();

	auto it = m_pEffectChannels.begin();
	while (it != m_pEffectChannels.end())
	{
		bool isPlaying = false;
		if (*it && (*it)->isPlaying(&isPlaying) == FMOD_OK && !isPlaying)
			it = m_pEffectChannels.erase(it);
		else
			++it;
	}
}

void Sound::Destroy()
{
	if (m_pSound)
	{
		m_pSound->release();
		m_pSound = nullptr;
	}

	m_pEffectChannels.clear();
	m_pChannel = nullptr;
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
	bool bPlay = false;
	if (m_pChannel)
		m_pChannel->isPlaying(&bPlay);

	if (!bPlay)
	{
		FMOD_RESULT hr = m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
		if (hr == FMOD_OK && m_pChannel)
		{
			m_pChannel->setVolume(m_fVolume);
			m_pChannel->setMode(bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		}
	}
}
void	Sound::PlayEffect2D(bool bLoop)
{
	FMOD::Channel* pNewChannel = nullptr;
	FMOD_RESULT hr = m_pSystem->playSound(m_pSound, nullptr, false, &pNewChannel);
	if (hr == FMOD_OK && pNewChannel)
	{
		pNewChannel->setVolume(m_fVolume);
		pNewChannel->setMode(bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		m_pEffectChannels.push_back(pNewChannel);
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

bool SoundManager::Load(ESoundType type, const std::wstring& path)
{
	if (m_pSystem == nullptr)
	{
		FMOD_RESULT hr = FMOD::System_Create(&m_pSystem);
		if (hr == FMOD_OK)
		{
			m_pSystem->init(32, FMOD_INIT_NORMAL, 0);
		}
	}

	if (m_Sounds[static_cast<int>(type)] != nullptr)
	{
		return true; // 이미 있으면 무시
	}

	Sound* pSound = new Sound();
	if (pSound->Load(m_pSystem, path))
	{
		m_Sounds[static_cast<int>(type)] = pSound;
		maplist[path] = pSound;
		return true;
	}
	else
	{
		delete pSound;
		return false;
	}

	
}

void SoundManager::LoadAllSounds()
{
	//BGM
	Load(ESoundType::Intro, L"../Resources/Sound/intro.mp3");
	Load(ESoundType::Stage0, L"../Resources/Sound/state0.mp3");
	Load(ESoundType::Boss1, L"../Resources/Sound/betty.mp3");
	Load(ESoundType::Ending, L"../Resources/Sound/ending.mp3");

	//Charactor
	Load(ESoundType::Walk, L"../Resources/Sound/hero_walk_footsteps_stone_louder.wav");
	Load(ESoundType::Dash, L"../Resources/Sound/hero_dash.wav");
	Load(ESoundType::Slash, L"../Resources/Sound/crow_sword1.mp3");
	Load(ESoundType::Hit, L"../Resources/Sound/hero_damaged.wav");
	Load(ESoundType::Die, L"../Resources/Sound/hero_damaged.wav");

	//Fire
	Load(ESoundType::Die, L"../Resources/Sound/FireLoop.wav");

	//Monster
	Load(ESoundType::Attack_Bat, L"../Resources/Sound/bat_attack.wav");
	Load(ESoundType::Enemy_Damaged, L"../Resources/Sound/enemy_damage.wav");

	//UI
	Load(ESoundType::Hover, L"../Resources/Sound/ui_hover.wav");
	Load(ESoundType::Click, L"../Resources/Sound/ui_save.wav");
}

Sound* SoundManager::GetPtr(ESoundType type)
{
	return m_Sounds[static_cast<int>(type)];
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
