#include "pch.h"
#include "UAnimInstance.h"
#include "Input.h"
#include "Timer.h"
#include "Device.h"


void UAnimInstance::Tick()
{
	animFrame += TIMER->GetDeltaTime() * 30.0f;
	if (animFrame == 0)
	{
		int a = 0;
	}
	if (animFrame >= animTrackList[currentAnimTrackIndex].animList[0].size())
	{
		animFrame = 0;
	}

	if (INPUT->GetButton(R))
	{
		animFrame = 0;
		currentAnimTrackIndex++;
		if (currentAnimTrackIndex >= animTrackList.size())
		{
			currentAnimTrackIndex = 0;
		}
		int a = animTrackList[currentAnimTrackIndex].animList[0].size();
		int b = 0;
	}
}

void UAnimInstance::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(CbAnimData);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	//D3D11_SUBRESOURCE_DATA pInitialData;
	//ZeroMemory(&pInitialData, sizeof(pInitialData));
	//pInitialData.pSysMem = &currentAnimList;
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, NULL, _constantBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return;
	}
	return;
}

void UAnimInstance::SetInverseBone(vector<vector<Matrix>> _boneList)
{
	inverseBindPose = _boneList;
}

void UAnimInstance::AddTrack(AnimList _animTrack)
{
	animTrackList.emplace_back(_animTrack);
}
