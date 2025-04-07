#pragma once
#include "Singleton.h"

enum { KEY_FREE = 0, KEY_PUSH, KEY_HOLD, KEY_UP };

enum GameKey
{
	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',

	Q = 'Q',
	E = 'E',
	Z = 'Z',
	C = 'C',

	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	EXIT = VK_ESCAPE,
	LSHIFT = VK_LSHIFT,
	SPACE = VK_SPACE,

	//¸¶¿ì½º
	LCLICK = VK_LBUTTON,
	RCLICk = VK_RBUTTON,
	GAMEKEY_SIZE,
};

class Input : public Singleton<Input>
{
public:
	vector<DWORD> keys;
public:
	DWORD _dwKeyState[256];
	POINT _mouse;
public:
	DWORD CheckKey(DWORD _dwKey);
	bool GetButton(GameKey _key);
	bool GetButtonDown(GameKey _key);
	bool GetButtonUp(GameKey _key);
	POINT GetMousePos();

public:
	void Init();
	void Frame() {};
	void Render() {};
	void Release() {};
};