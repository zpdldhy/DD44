#include "pch.h"
#include "TPlayer.h"
#include <algorithm>

static const StatUpgradeInfo g_StatTable[(int)EStatType::COUNT][5] =
{
	// HP (�ڽ�Ʈ, �ִ� ü��)
	{ {200, 1.0f, 0.f}, {220, 1.0f, 0.f}, {250, 1.0f, 0.f}, {290, 1.0f, 0.f}, {350, 2.0f, 0.f} },
	// MeleeDamage (�ڽ�Ʈ, ���� ���ݷ�, ���� ����)
	{ {80, 0.5f, 0.2f}, {100, 0.5f, 0.2f}, {130, 0.5f, 0.2f}, {170, 0.5f, 0.2f}, {200, 1.0f, 0.2f} },
	// MoveSpeed (�ڽ�Ʈ, �̵� �ӵ�, ������ �ӵ�)
	{ {150, 0.05f, 0.01f}, {170, 0.05f, 0.01f}, {200, 0.05f, 0.01f}, {250, 0.05f, 0.01f}, {300, 0.05f, 0.06f} },
	// RangedDamage (�ڽ�Ʈ, ���Ÿ� ���ݷ�, ȭ�� ����)
	{ {100, 0.2f, 1.0f}, {120, 0.2f, 1.0f}, {150, 0.2f, 1.0f}, {200, 0.2f, 1.0f}, {250, 1.2f, 2.0f} }
};

void TPlayer::IncArrowCount(int _count)
{
	arrowCount += _count;
	// �ִ� 4
	arrowCount = arrowCount >= (int)m_fArrowCount ? (int)m_fArrowCount : arrowCount;
}

void TPlayer::DecArrowCount(int _count)
{
	arrowCount -= _count;
	// �ּڰ� 0
	arrowCount = arrowCount <= 0 ? 0 : arrowCount;
}

void TPlayer::StartClimbing()
{
	isClimbing = true;
}

void TPlayer::StopClimbing()
{
	isClimbing = false;
}

bool TPlayer::SpendSoul(int _amount)
{
	int soul = m_iSoul;
	if ((soul - _amount) < 0)
	{
		return false;
	}
	else
	{
		m_iSoul -= _amount;
		return true;
	}
}

bool TPlayer::UpgradeStat(EStatType type)
{
	int idx = static_cast<int>(type);
	int lv = m_iStatLevel[idx];

	if (lv >= 5)
		return false;

	const auto& info = g_StatTable[idx][lv];

	if (!SpendSoul(info.soulCost))
		return false;

	switch (type)
	{
	case EStatType::HP:
		m_fMaxHP += info.delta1;
		break;
	case EStatType::MeleeDamage:
		m_fMeleeDamage += info.delta1;
		m_vMeleeRange += Vec3(info.delta2, info.delta2, info.delta2);
		break;
	case EStatType::MoveSpeed:
		m_fMoveSpeed += info.delta1;
		m_fRollSpeed += info.delta2;
		break;
	case EStatType::RangedDamage:
		m_fRangedDamage += info.delta1;
		m_fArrowCount += info.delta2;
		break;
	}

	++m_iStatLevel[idx];
	return true;
}
