#pragma once
#include "AActor.h"

class APawn : public AActor
{
public:
	APawn() = default;
	virtual ~APawn() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;
};

// APawn
// �÷��̾ AI�� ���� ������ Actor
// �Է�, ��Ʈ��