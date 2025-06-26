#include "pch.h"
#include "ASoulActor.h"
#include "Timer.h"
#include "ObjectManager.h"
#include "EffectManager.h"


Vec3 ASoulActor::Prepare(const Vec3& _pos, const Vec3& _baseVelocity, float _scale)
{
	m_fSpeed = 0.f;
	m_fArcTimer = 0.f;
	m_eState = SoulState::IdleWave;
	Vec3 pos = _pos;
	SetPosition(pos);
	Vec3 velocity = _baseVelocity;
	velocity.y += 5;
	velocity *= 5;
	SetGravity(0);
	SetStartScale(Vec3(1.5f) * _scale);
	SetEndScale(Vec3(1.5f) * _scale);
	InitSpriteAnimation(1, 0.6f);
	return velocity;


}

void ASoulActor::SetTarget(const Vec3& _targetPos)
{
	m_vTargetPos = _targetPos;
}


//void ASoulActor::Tick()
//{
//	if (!m_bRender)
//		return;
//
//	for (auto& iter : m_vCollisionList)
//	{
//		auto pActor = OBJECT->GetActor(iter.first);
//		if (!pActor) continue;
//
//
//		m_bRender = false;
//		break;
//	}
//	//�¿� ����
//	//{
//	//	Vec3 currentPos = GetPosition();
//	//	Vec3 toTarget = m_vTargetPos - currentPos;
//	//	float dist = toTarget.Length();
//
//	//	toTarget.Normalize();
//	//	Vec3 right = Vec3(-toTarget.z, 0, toTarget.x); // ���� ����
//	//	float wave = sin(TIMER->GetGameTime() * 10.f) * 3.5f; // ���� ����
//
//	//	Vec3 waveOffset = right * wave;
//
//	//	m_fSpeed += 1.0 * TIMER->GetDeltaTime();
//	//	m_vVelocity = (toTarget * m_fSpeed) + waveOffset;
//	//}
//
//	// ���Ʒ������� �¿����(������)
//	//{
//	//	Vec3 currentPos = GetPosition();
//	//	Vec3 toTarget = m_vTargetPos - currentPos;
//	//	float dist = toTarget.Length();
//
//	//	toTarget.Normalize();
//	//	Vec3 right = Vec3(-toTarget.z, 0, toTarget.x); // ���� ����
//
//	//	// ��鸲
//	//	float horizontalWave = sin(TIMER->GetGameTime() * 10.f) * 3.5f;
//	//	float verticalWave = sin(TIMER->GetGameTime() * 8.f) * 1.5f;
//
//	//	Vec3 waveOffset = right * horizontalWave + Vec3(0, verticalWave, 0);
//
//	//	// ���ӵ�
//	//	m_fSpeed += 1.0f * TIMER->GetDeltaTime();
//	//	m_vVelocity = toTarget * m_fSpeed + waveOffset;
//	//}
//
//	////	// ���ڸ����� ���Ʒ��� ����
//	{
//		Vec3 currentPos = GetPosition();
//		float verticalWave = sin(TIMER->GetGameTime() * 8.f) * 1.5f;
//		m_vVelocity = Vec3(0, verticalWave, 0); // ���Ʒ��� �̵�
//	}
//
//
//	AEffectActor::Tick();
//
//
//
//}
void ASoulActor::Tick()
{
	if (!m_bRender)
		return;

	// �浹 �� ������ �̵� ���·� ����
	for (auto& iter : m_vCollisionList)
	{
		for (auto& iter : m_vCollisionList)
		{
			auto pActor = OBJECT->GetActor(iter.first);
			if (!pActor) continue;


			m_bRender = false;
			break;
		}
	}

	switch (m_eState)
	{
	case SoulState::IdleWave:
	{
		float waveY = sin(TIMER->GetGameTime() * 8.f) * 1.5f;
		m_vVelocity = Vec3(0, waveY, 0);

		float distSoul = (m_vTargetPos - GetPosition()).Length();
		if (distSoul < 5.0f)
		{
			m_eState = SoulState::ArcMove;
			m_vArcStart = GetPosition();
			m_vArcEnd = m_vTargetPos;
			m_fArcTimer = 0.f;
		}
		break;
	}
	case SoulState::ArcMove:
	{
		m_vArcEnd = m_vTargetPos;

		m_fArcTimer += TIMER->GetDeltaTime();
		float t = m_fArcTimer / m_fArcDuration;


		// � ����
		Vec3 mid = (m_vArcStart + m_vArcEnd) * 0.5f + Vec3(0, 3.0f, 0); // �߰��� + ���
		Vec3 p1 = Lerp(m_vArcStart, mid, t);
		Vec3 p2 = Lerp(mid, m_vArcEnd, t);
		Vec3 curvedPos = Lerp(p1, p2, t);

		Vec3 scale = Lerp(m_vStartSoulScale, m_vEndSoulScale, t);
		SetScale(scale);

		m_vVelocity = (curvedPos - GetPosition()) / TIMER->GetDeltaTime();
		break;
	}
	}

	AEffectActor::Tick();
}