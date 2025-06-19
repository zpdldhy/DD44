#include "pch.h"
#include "AParticleActor.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "Timer.h"
#include "CameraManager.h"

AParticleActor::AParticleActor()
{
}

void AParticleActor::Tick()
{
	AActor::Tick();

	if (!m_bPlaying)
		return;

	float deltaTime = TIMER->GetDeltaTime();
	m_fTimeAcc += deltaTime;

	float frameTime = m_fDuration / (m_iDivisions * m_iDivisions);
	if (m_fTimeAcc >= frameTime)
	{
		m_fTimeAcc -= frameTime;

		int maxFrames = m_iDivisions * m_iDivisions;

		if (m_bReversePlay)
			m_iCurrentFrame--;
		else
			m_iCurrentFrame++;

		// 종료 조건 처리
		if (m_bReversePlay)
		{
			if (m_iCurrentFrame < 0)
			{
				if (m_bLoop)
					m_iCurrentFrame = maxFrames - 1;
				else
				{
					m_iCurrentFrame = 0;
					m_bPlaying = false;
					if (m_bAutoDestroy)
						m_bDelete = true;
				}
			}
		}
		else
		{
			if (m_iCurrentFrame >= maxFrames)
			{
				if (m_bLoop)
					m_iCurrentFrame = 0;
				else
				{
					m_iCurrentFrame = maxFrames - 1;
					m_bPlaying = false;
					if (m_bAutoDestroy)
						m_bDelete = true;
				}
			}
		}

		// UV 계산
		int row = m_iCurrentFrame / m_iDivisions;
		int col = m_iCurrentFrame % m_iDivisions;

		float cellSize = 1.0f / m_iDivisions;
		Vec2 uvStart(col * cellSize, row * cellSize);
		Vec2 uvEnd((col + 1) * cellSize, (row + 1) * cellSize);

		GetMeshComponent()->SetInstanceStartUV(uvStart);
		GetMeshComponent()->SetInstanceEndUV(uvEnd);
	}
}

void AParticleActor::Render()
{
	AActor::Render();
}

void AParticleActor::InitSpriteAnimation(int divisions, float duration)
{
	m_iDivisions = divisions;
	m_fDuration = duration;
	SetDuration(duration);
	
	int totalFrames = m_iDivisions * m_iDivisions;
	m_iCurrentFrame = m_bReversePlay ? totalFrames - 1 : 0;
	//m_fTimeAcc = RandomRange(0.0f, duration);
	m_fTimeAcc = 0.0f;
	m_bPlaying = true;
	m_bDelete = false;
}

void AParticleActor::SetDuration(float seconds)
{
	if (seconds <= 0.01f) return; // 방어 코드
	m_fDuration = seconds;
	int totalFrames = m_iDivisions * m_iDivisions;
	m_fFrameRate = totalFrames / m_fDuration;
}
