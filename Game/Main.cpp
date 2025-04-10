#include "Main.h"
#include "pch.h"
#include "Engine.h"
#include "Sample.h"
#include "TestSY.h"

int WINAPI wWinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow)
{
    shared_ptr<IExecute> app = make_shared<TestSY>();

    Engine game(_hInstance, app);
    game.Run();

}