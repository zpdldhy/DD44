#include "pch.h"
#include "AnimTrack.h"
#include "Device.h"

void AnimTrack::Render()
{
	// �ش� �����ӿ� ���� MATRIX ����
	for (int i = 0; i < animInstance->animTrackList[animInstance->currentAnimTrackIndex].animList.size(); i++)
	{
		currentAnimData.boneAnim[i] = animInstance->inverseBindPose[meshIndex][i] * animInstance->animTrackList[animInstance->currentAnimTrackIndex].animList[i][animInstance->animFrame];
		
		// ������ ��ġ ����
		if (animInstance->m_bInPlace)
		{
			Matrix inverse = Matrix::CreateTranslation(-(animInstance->rootPos));
			currentAnimData.boneAnim[i] = currentAnimData.boneAnim[i] * inverse;

		}
		currentAnimData.boneAnim[i] = currentAnimData.boneAnim[i].Transpose();
	}

	DC->UpdateSubresource(animInstance->_constantBuffer.Get(), 0, NULL, &currentAnimData, 0, 0);
	DC->VSSetConstantBuffers(5, 1, animInstance->_constantBuffer.GetAddressOf());
}
