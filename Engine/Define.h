#pragma once

#define GET_SINGLE(classname)	classname::GetInstance()
#define DEVICE		GET_SINGLE(Device)->GetDevice()
#define DC			GET_SINGLE(Device)->GetDeviceContext()
#define SHADER		GET_SINGLE(ShaderManager)
#define TEXTURE		GET_SINGLE(TextureManager)
#define INPUTLAYOUT GET_SINGLE(InputlayoutManager)
#define STATE		GET_SINGLE(DxState)
#define INPUT		GET_SINGLE(Input)
