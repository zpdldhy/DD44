#include "pch.h"
#include "UAnimInstance.h"
#include "Input.h"
#include "Timer.h"
#include "Device.h"


void UAnimInstance::Tick()
{
	if (!m_bPlay) { return; }
	animFrame += TIMER->GetDeltaTime() * m_fAnimPlayRate;

	UINT lastFrame = animTrackList[currentAnimTrackIndex].endFrame;

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
	if (currentFrame != lastEventFrame)
	{
		TriggetEvent(currentAnimTrackIndex, currentFrame);
		lastEventFrame = currentFrame;
	}
}

void UAnimInstance::Render()
{
	// 한 모델 공용
	DC->VSSetShaderResources(3, 1, m_pTexSRV.GetAddressOf());
}

shared_ptr<UAnimInstance> UAnimInstance::Clone()
{
	// 프레임 하나때문에 공용으로 못쓰네. 
	// 데이터랑 프레임 계산이랑 따로 나눠야되나
	shared_ptr<UAnimInstance> newAnim = make_shared<UAnimInstance>();

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
	newAnim->animTexData = animTexData;
	newAnim->CreateConstantBuffer();
	newAnim->CreateTex();

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

void UAnimInstance::CreateTex()
{
	texWidth = boneCount;
	texHeight = 150;
	texDepth = animTrackList.size() * 4;

	// Texture

	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = texWidth;
	desc.Height = texHeight;
	desc.Depth = texDepth;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = animTexData.data();
	initData.SysMemPitch = texWidth * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = texWidth * texHeight * sizeof(XMFLOAT4);

	HRESULT hr = DEVICE->CreateTexture3D(&desc, &initData, m_pTex3D.GetAddressOf());
	if (FAILED(hr))
	{
		assert(false);
	}

	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MipLevels = 1;

	hr = DEVICE->CreateShaderResourceView(m_pTex3D.Get(), &srvDesc, m_pTexSRV.GetAddressOf());
	if (FAILED(hr))
	{
		assert(false);
	}
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

	for (int row = 0; row < 4; ++row)
	{
		int z = _track * 4 + row;

		size_t index = _bone
			+ _frame * texWidth
			+ z * texWidth * texHeight;

		const XMFLOAT4& rowData = animTexData[index];

		ret.m[row][0] = rowData.x;
		ret.m[row][1] = rowData.y;
		ret.m[row][2] = rowData.z;
		ret.m[row][3] = rowData.w;
	}

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
