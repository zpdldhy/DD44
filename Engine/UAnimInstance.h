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
	wstring m_modelName;
	vector<AnimList> animTrackList;
	float animFrame = 0.0f;
	UINT currentAnimTrackIndex = 0;
	bool m_bInPlace;
	Vec3 rootPos;
	int rootIndex;
	int prevIndex;
	
	ComPtr<ID3D11Buffer> _constantBuffer;
public:
	friend class AnimTrack;
	bool m_bOnPlayOnce;
	bool m_bPlay = true;

public:
	void Tick();
	void CreateConstantBuffer();
	void AddTrack(AnimList _animTrack);
 	void SetRootIndex(int _index) { rootIndex = _index; }
	void CheckInPlace(bool _inPlace) { m_bInPlace = _inPlace; }
	int GetAnimIndex(wstring _animName);
	void SetCurrentAnimTrack(int _index);
	void PlayOnce(int _index);
	Matrix GetBoneAnim(int _boneIndex);
	void SetName(wstring _name) { m_modelName = _name; }
	wstring GetName() { return m_modelName; }
public:
	const std::vector<AnimList>& GetAnimTrackList() const { return animTrackList; }
	float m_fAnimPlayRate = 25.0f;
};

