#pragma once
#include "AActor.h"
struct CB_Billboard
{
	Vec3 center;
	float padding_center;
	Vec2 size;
	Vec2  padding_size;
};

class AParticleActor :  public AActor
{
public:
	AParticleActor();
	virtual ~AParticleActor() = default;

	virtual void Tick() override; // AActor���� ����� Tick()

	void SetUV(Vec2 start, Vec2 end);
	void InitSpriteAnimation(int divisions, float frameRate);
private:
	int m_iDivisions = 4;        // ���� �� (N x N)
	float m_fFrameRate = 10.0f;  // �ʴ� ������ ��
	float m_fTimeAcc = 0.0f;     // �ð� ����
	int m_iCurrentFrame = 0;     // ���� ������ �ε���
	bool m_bPlaying = false;
	Vec2 m_uvStart = { 0.0f, 0.0f };
	Vec2 m_uvEnd = { 1.0f, 1.0f };
	shared_ptr<ConstantBuffer<CB_Billboard>> m_CB_Billboard;
};

