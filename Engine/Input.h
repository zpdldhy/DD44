#pragma once
#include "Singleton.h"

enum
{
	KEY_FREE = 0,
	KEY_PUSH,
	KEY_HOLD,
	KEY_UP
};

enum GameKey
{
    // 알파벳
    A = 'A', B = 'B', C = 'C', D = 'D', E = 'E',
    F = 'F', G = 'G', H = 'H', I = 'I', J = 'J',
    K = 'K', L = 'L', M = 'M', N = 'N', O = 'O',
    P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T',
    U = 'U', V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

    // 숫자 키보드 (상단 숫자)
    NUM_1 = '1', NUM_2 = '2', NUM_3 = '3', NUM_4 = '4', NUM_5 = '5',
    NUM_6 = '6', NUM_7 = '7', NUM_8 = '8', NUM_9 = '9', NUM_0 = '0',

    // 방향키
    UP      = VK_UP,
    DOWN    = VK_DOWN,
    LEFT    = VK_LEFT,
    RIGHT   = VK_RIGHT,

    // 기능키
    ESC         = VK_ESCAPE,
    TAB         = VK_TAB,
    ENTER       = VK_RETURN,
    SPACE       = VK_SPACE,
    BACKSPACE   = VK_BACK,
    LSHIFT      = VK_LSHIFT,
    RSHIFT      = VK_RSHIFT,
    LCTRL       = VK_LCONTROL,
    RCTRL       = VK_RCONTROL,
    LALT        = VK_LMENU,
    RALT        = VK_RMENU,
    CAPSLOCK    = VK_CAPITAL,

    // 기능(F)키
    F1 = VK_F1, F2  = VK_F2,  F3  = VK_F3,  F4  = VK_F4,
    F5 = VK_F5, F6  = VK_F6,  F7  = VK_F7,  F8  = VK_F8,
    F9 = VK_F9, F10 = VK_F10, F11 = VK_F11, F12 = VK_F12,

    // 숫자패드 (Numpad)
    NP0 = VK_NUMPAD0,
    NP1 = VK_NUMPAD1, NP2 = VK_NUMPAD2, NP3 = VK_NUMPAD3,
    NP4 = VK_NUMPAD4, NP5 = VK_NUMPAD5, NP6 = VK_NUMPAD6,
    NP7 = VK_NUMPAD7, NP8 = VK_NUMPAD8, NP9 = VK_NUMPAD9,

    // 마우스 버튼
    LCLICK  = VK_LBUTTON,
    RCLICK  = VK_RBUTTON,
    MBUTTON = VK_MBUTTON,

    // 마우스 휠
    MWHEEL_UP,    // 커스텀 정의 필요
    MWHEEL_DOWN,  // 커스텀 정의 필요

    // 기타
    KEY_INSERT      = VK_INSERT,
    KEY_DELETE      = VK_DELETE,
    KEY_HOME        = VK_HOME,
    KEY_END         = VK_END,
    KEY_PAGEUP      = VK_PRIOR,
    KEY_PAGEDOWN    = VK_NEXT,
    KEY_PAUSE       = VK_PAUSE,
    KEY_PRINT       = VK_PRINT,
    KEY_SCROLLLOCK  = VK_SCROLL,
};


class Input : public Singleton<Input>
{
public:
	vector<DWORD> keys;
public:
	DWORD _dwKeyState[256] = { 0, };
	POINT _mouse = { 0, 0 };
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