#pragma once
#include "Singleton.h"
#include "FMod/fmod.hpp"
#include "FMod/fmod_errors.h"
#pragma comment(lib, "FMod/fmod_vc.lib")


enum class ESoundType
{
	Intro,
	Stage0,
	Boss1,
	Stage1,
	Boss2,
	STage2,
	Die,
	Ending,

	//Charactor
	Slash,
	Dash,
	Walk,
	Hit,

	//Bat
	Attack_Bat,
	Enemy_Damaged,

	//UI
	Hover,
	Click,

	//Fire
	Fire,

	Max


};


class Sound
{
private:
	FMOD::System* m_pSystem = nullptr;
	FMOD::Sound* m_pSound = nullptr;

	// BGM용 채널
	FMOD::Channel* m_pChannel = nullptr;

	// 효과음용 채널리스트
	std::vector<FMOD::Channel*> m_pEffectChannels;

public:
	float m_fVolume = 0.5f;
	std::wstring m_csBuffer;
	std::wstring m_csName;
	bool Load(FMOD::System* pSystem, std::wstring filename);

public:
	void Play2D(bool bLoop = true);
	void PlayEffect2D(bool bLoop = false);
	
	void Stop();
	void Paused();
	void VolumeUp(float fVolume);
	void VolumeDown(float fVolume);

public:
	void Update();
	void Destroy();

public:
	Sound() : m_csName(L"none") {}
	Sound(std::wstring key) : m_csName(key) {}

};






class SoundManager : public Singleton<SoundManager>
{
	friend class Singleton<SoundManager>;

private:
	FMOD::System* m_pSystem = nullptr;
	Sound* m_Sounds[static_cast<int>(ESoundType::Max)] = { nullptr, };

public:
	bool    Load(ESoundType type, const std::wstring& path);
	void    LoadAllSounds();
	Sound*  GetPtr(ESoundType type);
	void    Update();

private:
	std::map<std::wstring, Sound*> maplist;
	SoundManager() {}
	~SoundManager();
};