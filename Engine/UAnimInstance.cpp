#include "pch.h"
#include "UAnimInstance.h"
#include "Input.h"
#include "Timer.h"
#include "Device.h"


void UAnimInstance::Tick()
{
	if (!m_bPlay) { return; }
	animFrame += TIMER->GetDeltaTime() * m_fAnimPlayRate;

	UINT lastFrame = animTrackList[currentAnimTrackIndex].animList[0].size();

	auto iter = m_mKeyFrameMap.find(currentAnimTrackIndex);
	bool hasEnd = false;
	if (iter != m_mKeyFrameMap.end())
	{
		hasEnd = true;
		lastFrame = iter->second;
	}


	if (animFrame >= lastFrame)
	{
		m_bOnPlayOnce = false;
		if (!hasEnd)
		{
			if (m_bOnPlayOnce)
			{
				currentAnimTrackIndex = prevIndex;
				m_bOnPlayOnce = false;
			}
			animFrame = 0;
		}
		else
		{
			animFrame = lastFrame;
		}
	}

	// ROOTMOTION
	if (m_bInPlace)
	{
		rootPos.x = animTrackList[currentAnimTrackIndex].animList[rootIndex][animFrame]._41;
		rootPos.y = animTrackList[currentAnimTrackIndex].animList[rootIndex][animFrame]._42;
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

	// Event
	// should play event only once per frame
	UINT currentFrame = static_cast<UINT>(animFrame);
	if (currentFrame != lastEventFrame)
	{
		TriggetEvent(currentAnimTrackIndex, currentFrame);
		lastEventFrame = currentFrame;
	}
}

shared_ptr<UAnimInstance> UAnimInstance::Clone()
{
	shared_ptr<UAnimInstance> newAnim = make_shared<UAnimInstance>();

	newAnim->m_modelName = m_modelName;
	newAnim->animTrackList = animTrackList;
	newAnim->animFrame = animFrame;
	newAnim->currentAnimTrackIndex = currentAnimTrackIndex;
	newAnim->rootPos = rootPos;
	newAnim->rootIndex = rootIndex;
	newAnim->prevIndex = prevIndex;
	newAnim->m_bOnPlayOnce = m_bOnPlayOnce;
	newAnim->m_bInPlace = m_bInPlace;
	newAnim->m_bPlay = m_bPlay;
	newAnim->m_fAnimPlayRate = m_fAnimPlayRate;

	newAnim->CreateConstantBuffer();

	return newAnim;
}

void UAnimInstance::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(CbAnimData);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

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
	if (m_bOnPlayOnce) { return; }
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
	currentAnimTrackIndex = _index;
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
	Matrix ret = animTrackList[currentAnimTrackIndex].animList[_boneIndex][animFrame];
	if (_boneIndex >= animTrackList[currentAnimTrackIndex].animList.size())
	{
		return Matrix();
	}

	if (m_bInPlace)
	{
		Matrix inverse = Matrix::CreateTranslation(-rootPos);
		ret = ret * inverse;
	}

	return ret;
}

// set end frame
void UAnimInstance::SetKeyFrame(int _trackIndex, UINT _key)
{
	if (_key >= animTrackList[_trackIndex].animList[0].size() || _key < 0)
	{
		return;
	}

	m_mKeyFrameMap.insert(make_pair(_trackIndex, _key));

}

void UAnimInstance::AddEvent(int _trackIndex, UINT _keyFrame, function<void()> _func)
{
	eventMap[{_trackIndex, _keyFrame}] = _func;
}

void UAnimInstance::TriggetEvent(int _trackIndex, UINT _currentFrame)
{
	auto it = eventMap.find({ _trackIndex, _currentFrame });
	if (it != eventMap.end()) {
		it->second();
	}
}
