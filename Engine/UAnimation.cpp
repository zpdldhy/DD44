#include "pch.h"
#include "UAnimation.h"
#include "Device.h"
#include "Timer.h"
#include "Input.h"

void UAnimation::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(CbAnimData);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA pInitialData;
	ZeroMemory(&pInitialData, sizeof(pInitialData));
	pInitialData.pSysMem = &currentAnimList;
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, &pInitialData, _constantBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return;
	}
	return;

}

void UAnimation::Tick()
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
		currentAnimTrackIndex++;
		if (currentAnimTrackIndex >= animTrackList.size())
		{
			currentAnimTrackIndex = 0;
		}
	}

}

void UAnimation::Render(int iChild)
{

	// 해당 프레임에 대한 MATRIX 수집
	for (int i = 0; i < animTrackList[currentAnimTrackIndex].animList.size(); i++)
	{
		currentAnimList.boneAnim[i] = inversedBone[i] * animTrackList[currentAnimTrackIndex].animList[i][animFrame];
		currentAnimList.boneAnim[i] = currentAnimList.boneAnim[i].Transpose();
	}

	DC->UpdateSubresource(_constantBuffer.Get(), 0, NULL, &currentAnimList, 0, 0);
	DC->VSSetConstantBuffers(3, 1, _constantBuffer.GetAddressOf());
}
