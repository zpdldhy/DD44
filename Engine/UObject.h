#pragma once

class UObject
{
public:
	UObject() = default;
	virtual ~UObject() = default;

protected:
	wstring m_Name;

public:
	virtual void Init() abstract;
	virtual void Tick() abstract;
	virtual void Render() abstract;
	virtual void Destroy() abstract;

public:
	wstring& GetName() { return m_Name; }
	void SetName(const wstring& _name) { m_Name = _name; }
};

// UObject
// 리플렉션, 직렬화, 기본 기능 제공