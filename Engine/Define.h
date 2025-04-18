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