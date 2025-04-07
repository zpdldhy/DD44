#include "pch.h"
#include "Input.h"

DWORD Input::CheckKey(DWORD _dwKey)
{
	SHORT sKey = GetAsyncKeyState(_dwKey);
	if (sKey & 0x8000 || sKey & 0x8001)
	{
		if (_dwKeyState[_dwKey] == KEY_FREE ||
			_dwKeyState[_dwKey] == KEY_UP)
		{
			_dwKeyState[_dwKey] = KEY_PUSH;
		}
		else
		{
			_dwKeyState[_dwKey] = KEY_HOLD;
		}
	}
	else
	{
		if (_dwKeyState[_dwKey] == KEY_PUSH ||
			_dwKeyState[_dwKey] == KEY_HOLD)
		{
			_dwKeyState[_dwKey] = KEY_UP;
		}
		else
		{
			_dwKeyState[_dwKey] = KEY_FREE;
		}
	}
	return _dwKeyState[_dwKey];
}

bool Input::GetButton(GameKey _key)
{
	return CheckKey(_key) == KEY_PUSH;
}

bool Input::GetButtonDown(GameKey _key)
{
	return CheckKey(_key) == KEY_HOLD;
}

bool Input::GetButtonUp(GameKey _key)
{
	return CheckKey(_key) == KEY_UP;
}

POINT Input::GetMousePos()
{
	GetCursorPos(&_mouse);
	ScreenToClient(g_hWnd, &_mouse);
	return _mouse;
}

void Input::Init()
{
	ZeroMemory(_dwKeyState, sizeof(DWORD) * 256);
}