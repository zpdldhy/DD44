#pragma once

#define GET_SINGLE(classname)	classname::GetInstance()
#define DEVICE		GET_SINGLE(Device)->GetDevice()
#define DC			GET_SINGLE(Device)->GetDeviceContext()
#define SWAPCHAIN   GET_SINGLE(Device)->GetSwapChain()
#define SHADER		GET_SINGLE(ShaderManager)
#define TEXTURE		GET_SINGLE(TextureManager)
#define INPUTLAYOUT GET_SINGLE(InputlayoutManager)
#define STATE		GET_SINGLE(DxState)
#define INPUT		GET_SINGLE(Input)
#define DXWRITE     GET_SINGLE(DxWrite)
#define TYPER		GET_SINGLE(Typer)
#define TIMER		GET_SINGLE(Timer)
#define GUI			GET_SINGLE(ImGuiCore)
#define SOUNDMANAGER	GET_SINGLE(SoundManager)
#define CAMERAMANAGER	GET_SINGLE(CameraManager)
#define UIMANAGER		GET_SINGLE(UIManager)
#define OBJECTMANAGER	GET_SINGLE(ObjectManager)
#define PREFAB      GET_SINGLE(PrefabLoader)
#define LIGHTMANAGER GET_SINGLE(LightManager)
#define POSTPROCESS  GET_SINGLE(PostProcessManager)

#define DD_PI		3.1415926f