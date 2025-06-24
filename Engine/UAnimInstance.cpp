#include "pch.h"
#include "UAnimInstance.h"
#include "Input.h"
#include "Timer.h"



void UAnimInstance::Tick()
{
	if (!m_bPlay) { return; }
	animFrame += TIMER->GetDeltaTime() * m_fAnimPlayRate;

	UINT lastFrame = animTrackList[currentAnimTrackIndex].endFrame - 1;

	auto iter = m_mKeyFrameMap.find(currentAnimTrackIndex);
	bool hasEnd = false;
	if (iter != m_mKeyFrameMap.end())
	{
		hasEnd = true;
		lastFrame = iter->second;
	}


	if (animFrame >= lastFrame)
	{
		if (!hasEnd)
		{
			if (m_bOnPlayOnce)
			{
				//currentAnimTrackIndex = prevIndex;
				m_bOnPlayOnce = false;
				animFrame = lastFrame - 1;
			}
			else
			{
				animFrame = 0;
			}
		}
		else
		{
			m_bOnPlayOnce = false;
			animFrame = lastFrame;
		}
	}

	// ROOTMOTION
	if (m_bInPlace)
	{
		rootMat = GetMatrix(currentAnimTrackIndex, rootIndex, animFrame);

		rootPos.x = rootMat._41;
		rootPos.y = rootMat._42;
		rootPos.z = rootMat._43;
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
	for (UINT frame = lastEventFrame + 1; frame <= currentFrame; ++frame) {
		TriggetEvent(currentAnimTrackIndex, frame);
	}
	lastEventFrame = currentFrame;
}

void UAnimInstance::Render()
{
	m_pData->Bind();
}

shared_ptr<UAnimInstance> UAnimInstance::Clone()
{
	shared_ptr<UAnimInstance> newAnim = make_shared<UAnimInstance>();

	newAnim->m_pData = m_pData;

	newAnim->m_modelName = m_modelName;
	newAnim->boneCount = boneCount;
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

	return newAnim;
}

void UAnimInstance::MakeAnimData(const vector<XMFLOAT4>& _data)
{
	m_pData = make_shared<AnimData>();
	m_pData->AddData(_data);
	m_pData->CreateTex(boneCount, animTrackList.size());
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
	prevIndex = currentAnimTrackIndex;
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
	if (_boneIndex >= boneCount)
	{
		return Matrix();
	}

	Matrix ret = GetMatrix(currentAnimTrackIndex, _boneIndex, animFrame);

	if (m_bInPlace)
	{
		Matrix inverse = Matrix::CreateTranslation(-rootPos);
		ret = ret * inverse;
	}

	return ret;
}

Matrix UAnimInstance::GetMatrix(int _track, int _bone, int _frame)
{
	Matrix ret;

	if (_bone >= boneCount || _frame >= animTrackList[_track].endFrame)
	{
		return ret;
	}

	ret = m_pData->GetMatrix(_track, _bone, _frame);

	return ret;
}

void UAnimInstance::SetBoneCount(UINT _bone)
{
	boneCount = _bone;
}

// set end frame
void UAnimInstance::SetKeyFrame(int _trackIndex, UINT _key)
{
	if (_key >= animTrackList[_trackIndex].endFrame || _key < 0)
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
