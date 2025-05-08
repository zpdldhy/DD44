#include "pch.h"
#include "UAnimInstance.h"
#include "Input.h"
#include "Timer.h"
#include "Device.h"


void UAnimInstance::Tick()
{
	animFrame += TIMER->GetDeltaTime() * m_fAnimPlayRate;
	
	if (animFrame >= animTrackList[currentAnimTrackIndex].animList[0].size())
	{
		if (m_bOnPlayOnce)
		{
			currentAnimTrackIndex = prevIndex;
			m_bOnPlayOnce = false;
		}
		animFrame = 0;
	}

	// ROOTMOTION üũ
	if (m_bInPlace)
	{
		rootPos.x = animTrackList[currentAnimTrackIndex].animList[rootIndex][animFrame]._41;
		//rootPos.y = animTrackList[currentAnimTrackIndex].animList[4][animFrame]._42;
		rootPos.z = animTrackList[currentAnimTrackIndex].animList[rootIndex][animFrame]._43;

	}

	
	// 애니메이션 파싱 확인 용
	/*if (INPUT->GetButton(R))
	{
		animFrame = 0;
		currentAnimTrackIndex++;
		if (currentAnimTrackIndex >= animTrackList.size())
		{
			currentAnimTrackIndex = 0;
		}
	}*/
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

void UAnimInstance::AddTrack(AnimList _animTrack)
{
	animTrackList.emplace_back(_animTrack);
}

int UAnimInstance::GetAnimIndex(wstring _animName)
{
	for (int iTrack = 0; iTrack < animTrackList.size(); iTrack++)
	{
		if (animTrackList[iTrack].m_szName.compare(_animName) == 0)
		{
			return iTrack;
		}
	}
	return 0;
}

void UAnimInstance::SetCurrentAnimTrack(int _index)
{
	if(m_bOnPlayOnce) { return; }
	if (_index < 0 || _index >= animTrackList.size())
	{
		return;
	}
	// 이 부분을 애니메이션 활용에 따라 다를 것 같아서 추가 구상 필요
	if (currentAnimTrackIndex == _index)
	{
		return;
	}
	animFrame = 0;
	currentAnimTrackIndex  =  _index;
}

void UAnimInstance::PlayOnce(int _index)
{
	animFrame = 0;
	prevIndex = currentAnimTrackIndex;
	currentAnimTrackIndex = _index;
	m_bOnPlayOnce = true;
}

Matrix UAnimInstance::GetBoneAnim(int _boneIndex)
{
	if (_boneIndex >= animTrackList[currentAnimTrackIndex].animList.size())
	{
		return Matrix();
	}
	return animTrackList[currentAnimTrackIndex].animList[_boneIndex][animFrame];
}
