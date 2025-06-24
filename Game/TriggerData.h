#pragma once

enum class ETriggerType
{
	TT_NONE,
	TT_LADDER,
	TT_HEALPOINT,	// 먹었을 때 아니라, 먹는 곳 앞에 서 있을 때
	TT_COUNT,
};

struct TriggerData
{
	ETriggerType eTriggerType = ETriggerType::TT_NONE;
	Vec3 vPoint = { 0.f, 0.f, 0.f };	// 어디서 일어나는지
};