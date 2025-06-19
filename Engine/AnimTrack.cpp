#include "pch.h"
#include "AnimTrack.h"
#include "Device.h"

void AnimTrack::Render()
{
	animInstance->cbData.frame = animInstance->animFrame;
	animInstance->cbData.track = animInstance->currentAnimTrackIndex;
	DC->UpdateSubresource(animInstance->_constantBuffer.Get(), 0, NULL, &animInstance->cbData, 0, 0);
	DC->VSSetConstantBuffers(5, 1, animInstance->_constantBuffer.GetAddressOf());
	//// 해당 프레임에 대한 MATRIX 수집
	//for (int i = 0; i < animInstance->animTrackList[animInstance->currentAnimTrackIndex].animList.size(); i++)
	//{
	//	//currentAnimData.boneAnim[i] = animInstance->inverseBindPose[meshIndex][i] * animInstance->animTrackList[animInstance->currentAnimTrackIndex].animList[i][animInstance->animFrame];
	//	currentAnimData.boneAnim[i] = animInstance->animTrackList[animInstance->currentAnimTrackIndex].animList[i][animInstance->animFrame];
	//	
	//	// 원점에 위치 고정
	//	if (animInstance->m_bInPlace)
	//	{
	//		Matrix inverse = Matrix::CreateTranslation(-(animInstance->rootPos));
	//		currentAnimData.boneAnim[i] = currentAnimData.boneAnim[i] * inverse;

	//	}
	//	//currentAnimData.boneAnim[i] = currentAnimData.boneAnim[i].Transpose();
	//}

	//DC->UpdateSubresource(animInstance->_constantBuffer.Get(), 0, NULL, &currentAnimData, 0, 0);
	//DC->VSSetConstantBuffers(5, 1, animInstance->_constantBuffer.GetAddressOf());
}
