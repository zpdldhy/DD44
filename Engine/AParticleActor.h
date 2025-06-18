#pragma once
#include "AActor.h"

class AParticleActor :  public AActor
{
public:
	AParticleActor();
	virtual ~AParticleActor() = default;

	virtual void Tick() override; // AActor에서 상속한 Tick()
	void Render() override;

	void SetUV(Vec2 start, Vec2 end);
	void InitSpriteAnimation(int divisions, float duration);

	void SetDuration(float seconds);
	float GetDuration() const { return m_fDuration; }
protected:
	int m_iDivisions = 4;        // 분할 수 (N x N)
	float m_fFrameRate = 10.0f;  // 초당 프레임 수
	float m_fTimeAcc = 0.0f;     // 시간 누적
	float m_fDuration = 1.0f;	 // 지속 시간	
	int m_iCurrentFrame = 0;     // 현재 프레임 인덱스
	bool m_bPlaying = false;
	Vec2 m_uvStart = { 0.0f, 0.0f };
	Vec2 m_uvEnd = { 1.0f, 1.0f };
	bool m_bReversePlay = false;

public:
	bool m_bLoop = true;
	bool m_bAutoDestroy = false;
};

