#include "Main.h"
#include "pch.h"
#include "Engine.h"
#include "Sample.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    shared_ptr<IExecute> app = make_shared<Sample>();
    Engine game(hInstance, app);
    game.Run();

}