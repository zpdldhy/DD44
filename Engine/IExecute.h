#pragma once
enum class SCENE_TYPE
{
	NON_GAME = 0,
	GAME = 1,
};

class IExecute
{
public:
	SCENE_TYPE m_type = SCENE_TYPE::NON_GAME;
public:
	virtual void Init() abstract;
	virtual void Tick() abstract;
	virtual void Render() abstract;
	virtual void Destroy() abstract;
};
