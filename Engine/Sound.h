#pragma once
#include "Singleton.h"
#include "FMod/fmod.hpp"
#include "FMod/fmod_errors.h"
#pragma comment(lib, "FMod/fmod_vc.lib")


enum class ESoundType
{
	Bomb,
	Bgm,
	Hit,
	Die,
	Max
};


class Sound
{
private:
	FMOD::System* m_pSystem = nullptr;
	FMOD::Sound* m_pSound = nullptr;
	FMOD::Channel* m_pChannel = nullptr;

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
	FMOD::System* m_pSystem = nullptr;

public:
	Sound* Load(std::wstring filename);
	Sound* GetPtr(std::wstring key);
	void    Update();

private:
	std::map<std::wstring, Sound*> maplist;
	SoundManager() {}
	~SoundManager();
};