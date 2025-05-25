#include "Main.h"
#include "pch.h"
#include "Engine.h"
#include "Sample.h"
#include "Game.h"
#include "Editor.h"

// 삭제 예정
#include "TestYR.h"
#include "TestSY.h"
#include "TestSJ.h"
#include "TestPlayer.h"
#include "TestYR2.h"
#include "TestYoooooon.h"

int WINAPI wWinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow)
{
    shared_ptr<IExecute> app = make_shared<TestYoooooon>();

    Engine game(_hInstance, app);
    game.Run();

    return 0;
}