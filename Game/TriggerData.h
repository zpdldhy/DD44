#pragma once

enum class ETriggerType
{
	TT_NONE,
	TT_LADDER,
	TT_HEALPOINT,	// �Ծ��� �� �ƴ϶�, �Դ� �� �տ� �� ���� ��
	TT_COUNT,
};

struct TriggerData
{
	ETriggerType eTriggerType = ETriggerType::TT_NONE;
	Vec3 vPoint = { 0.f, 0.f, 0.f };	// ��� �Ͼ����
};