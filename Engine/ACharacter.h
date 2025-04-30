#pragma once
#include "APawn.h"

class ACharacter : public APawn
{
public:
	ACharacter() = default;
	virtual ~ACharacter() = default;
public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;
};

// ACharacter
// ���̷��� �Ž� �߰�
// �̵�, ����, ������, ����