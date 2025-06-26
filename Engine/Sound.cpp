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
		if (!*it || (*it)->isPlaying(&isPlaying) != FMOD_OK || !isPlaying)
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
	FMOD_RESULT hr = m_pSystem->createSound(
		to_wm(_filename).c_str(),
		FMOD_2D | FMOD_CREATESAMPLE,
		0,
		&m_pSound
	);
	return hr == FMOD_OK;
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
void Sound::PlayEffect2D(bool bLoop)
{
	FMOD::Channel* pNewChannel = nullptr;
	FMOD_RESULT hr = m_pSystem->playSound(m_pSound, nullptr, false, &pNewChannel);
	if (hr == FMOD_OK && pNewChannel)
	{
		pNewChannel->setVolume(m_fVolume);
		pNewChannel->setMode(bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		pNewChannel->setPriority(255);
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
	Load(ESoundType::Boss1, L"../Resources/Sound/boss_betty.mp3");
	Load(ESoundType::Ending, L"../Resources/Sound/ending.mp3");

	//Charactor
	Load(ESoundType::Walk, L"../Resources/Sound/hero_walk_footsteps_stone_louder.wav");
	Load(ESoundType::Dash, L"../Resources/Sound/hero_dash.wav");
	Load(ESoundType::Slash, L"../Resources/Sound/crow_sword1.mp3");
	Load(ESoundType::Slash_2, L"../Resources/Sound/crow_sword2.mp3");
	Load(ESoundType::Slash_3, L"../Resources/Sound/crow_sword3.mp3");
	Load(ESoundType::Bow_Stretch, L"../Resources/Sound/bow_stretch.mp3");
	Load(ESoundType::Bow_Release, L"../Resources/Sound/bow_release.mp3");
	Load(ESoundType::Ladder, L"../Resources/Sound/ladder.mp3");
	Load(ESoundType::Hit, L"../Resources/Sound/hero_damaged.wav");
	Load(ESoundType::Die, L"../Resources/Sound/hero_damaged.wav");

	//Fire
	Load(ESoundType::Die, L"../Resources/Sound/FireLoop.wav");

	//Monster
	Load(ESoundType::Attack_Bat, L"../Resources/Sound/bat_attack.wav");
	Load(ESoundType::Enemy_Damaged, L"../Resources/Sound/enemy_damage.wav");
	Load(ESoundType::Impact, L"../Resources/Sound/monster_impact.mp3");
	Load(ESoundType::Dead_Walker, L"../Resources/Sound/zombie_dead.mp3");
	Load(ESoundType::Attack_Mage, L"../Resources/Sound/mage_magic.mp3");
	Load(ESoundType::Attack_Betty, L"../Resources/Sound/betty_attack.mp3");
	Load(ESoundType::Screaming_Betty, L"../Resources/Sound/betty_screaming.mp3");
	Load(ESoundType::Growling_Betty, L"../Resources/Sound/betty_growling.mp3");
	Load(ESoundType::Impact_Betty, L"../Resources/Sound/betty_impact.mp3");
	Load(ESoundType::Scraping_Betty, L"../Resources/Sound/betty_scraping.mp3");

	//Env
	Load(ESoundType::Open_Fence, L"../Resources/Sound/fence_open.mp3");
	Load(ESoundType::Close_Fence, L"../Resources/Sound/fence_close.mp3");
	Load(ESoundType::GetSoul, L"../Resources/Sound/soul_get.mp3");
	Load(ESoundType::Healing, L"../Resources/Sound/healing.mp3");

	//UI
	Load(ESoundType::Hover, L"../Resources/Sound/ui_hover.wav");
	Load(ESoundType::Click, L"../Resources/Sound/ui_save.wav");
	Load(ESoundType::Allow, L"../Resources/Sound/ui_levelup.mp3");
	Load(ESoundType::Deny, L"../Resources/Sound/ui_deny.mp3");
	Load(ESoundType::CrowDead, L"../Resources/Sound/ui_dead.mp3");
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
