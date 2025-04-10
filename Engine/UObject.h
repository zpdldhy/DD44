#pragma once

class UObject
{
public:
	UObject() = default;
	virtual ~UObject() = default;

public:
	virtual void Init();
	virtual void Tick();
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	virtual void Destroy();
};

// UObject
// ���÷���, ����ȭ, �⺻ ��� ����