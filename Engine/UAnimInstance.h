#pragma once
struct AnimList
{
	vector<vector<Matrix>> animList;
	wstring m_szName;
};

struct CbAnimData
{
	Matrix boneAnim[250];
};


class UAnimInstance
{
	vector<AnimList> animTrackList;
	vector<vector<Matrix>> inverseBindPose;
	float animFrame = 0.0f;
	UINT currentAnimTrackIndex = 0;
	bool m_bInPlace;
	Vec3 rootPos;
	int rootIndex;
	
	ComPtr<ID3D11Buffer> _constantBuffer;
public:
	friend class AnimTrack;

public:
	void Tick();
	void CreateConstantBuffer();
	void SetInverseBone(vector<vector<Matrix>> _boneList);
	void AddTrack(AnimList _animTrack);
	void SetRootIndex(int _index) { rootIndex = _index; }
	void CheckInPlace(bool _inPlace) { m_bInPlace = _inPlace; }
};

