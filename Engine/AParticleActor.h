#pragma once
#include "AActor.h"

class AParticleActor :  public AActor
{
public:
	AParticleActor();
	virtual ~AParticleActor() = default;

	virtual void Tick() override; // AActor���� ����� Tick()
	void Render() override;

	void SetUV(Vec2 start, Vec2 end);
	void InitSpriteAnimation(int divisions, float duration);

	void SetDuration(float seconds);
	float GetDuration() const { return m_fDuration; }
protected:
	int m_iDivisions = 4;        // ���� �� (N x N)
	float m_fFrameRate = 10.0f;  // �ʴ� ������ ��
	float m_fTimeAcc = 0.0f;     // �ð� ����
	float m_fDuration = 1.0f;	 // ���� �ð�	
	int m_iCurrentFrame = 0;     // ���� ������ �ε���
	bool m_bPlaying = false;
	Vec2 m_uvStart = { 0.0f, 0.0f };
	Vec2 m_uvEnd = { 1.0f, 1.0f };
	bool m_bReversePlay = false;

public:
	bool m_bLoop = true;
	bool m_bAutoDestroy = false;
};

