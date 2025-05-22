#include "pch.h"
#include "AParticleActor.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "Timer.h"
#include "CameraManager.h"

AParticleActor::AParticleActor()
{
	m_pTransform = make_shared<USceneComponent>();
	m_CB_Billboard = make_shared<ConstantBuffer<CB_Billboard>>();
	m_CB_Billboard->Create(6); // b6 슬롯
}

void AParticleActor::Tick()
{
	AActor::Tick();


	float deltaTime = TIMER->GetDeltaTime();
	m_fTimeAcc += deltaTime;

	float frameTime = m_fDuration / (m_iDivisions * m_iDivisions);
	if (m_fTimeAcc >= frameTime)
	{
		m_fTimeAcc -= frameTime;

		int maxFrames = m_iDivisions * m_iDivisions;

		// 루프 중이거나 아직 끝나지 않았으면 진행
		if (m_iCurrentFrame < maxFrames || m_bLoop)
		{
			m_iCurrentFrame++;

			if (m_iCurrentFrame >= maxFrames)
			{
				if (m_bLoop)
				{
					m_iCurrentFrame = 0;
				}
				else
				{
					m_iCurrentFrame = maxFrames - 1;
					m_bPlaying = false;
					if (m_bAutoDestroy)
					{
						m_bDelete = true;
					}
				}
			}

			int row = m_iCurrentFrame / m_iDivisions;
			int col = m_iCurrentFrame % m_iDivisions;

			float cellSize = 1.0f / m_iDivisions;
			Vec2 uvStart(col * cellSize, row * cellSize);
			Vec2 uvEnd((col + 1) * cellSize, (row + 1) * cellSize);

			SetUV(uvStart, uvEnd);
		}
	}
}

void AParticleActor::Render()
{
	if (m_CB_Billboard)
	{
		Vec3 scale = GetScale();
		m_CB_Billboard->data.center = GetPosition();
		m_CB_Billboard->data.size = Vec2(scale.x, scale.y); // uniform scale
		m_CB_Billboard->Update();
		m_CB_Billboard->Push();
	}
	AActor::Render();
}

void AParticleActor::SetUV(Vec2 start, Vec2 end)
{
	m_uvStart = start;
	m_uvEnd = end;

	auto mesh = GetMeshComponent();
	auto mat = mesh->GetMaterial();
	mat->SetUVRange(m_uvStart, m_uvEnd);
}

//void AParticleActor::InitSpriteAnimation(int divisions, float frameRate)
//{
//    m_iDivisions = divisions;
//    m_fFrameRate = frameRate;
//
//    m_iCurrentFrame = 0;
//    m_fTimeAcc = 0.0f;
//    m_bPlaying = true;
//}

void AParticleActor::InitSpriteAnimation(int divisions, float duration)
{
	m_iDivisions = divisions;
	m_fDuration = duration;
	SetDuration(duration);
	m_iCurrentFrame = 0;
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
