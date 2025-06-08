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

class UAnimInstance : public enable_shared_from_this<UAnimInstance>
{
	wstring m_modelName;
	vector<AnimList> animTrackList;
	float animFrame = 0.0f;
	UINT currentAnimTrackIndex = 0;
	Vec3 rootPos;
	int rootIndex = 0;
	int prevIndex;
	
	ComPtr<ID3D11Buffer> _constantBuffer;
public:
	friend class AnimTrack;
	bool m_bOnPlayOnce;
	bool m_bInPlace = false;
	bool m_bPlay = true;
	float m_fAnimPlayRate = 25.0f;
	map<int, UINT> m_mKeyFrameMap;
public:
	void CreateConstantBuffer();
	void PlayOnce(int _index);
public:
	void Tick();
	shared_ptr<UAnimInstance> Clone();
public:
	void AddTrack(AnimList _animTrack);
	void SetName(wstring _name) { m_modelName = _name; }
	wstring GetName() { return m_modelName; }
	void CheckInPlace(bool _inPlace) { m_bInPlace = _inPlace; }
 	void SetRootIndex(int _index) { rootIndex = _index; }
	int GetRootIndex() { return rootIndex; }
	void SetCurrentAnimTrack(int _index);
	int GetAnimIndex(wstring _animName);
	Matrix GetBoneAnim(int _boneIndex);
	void SetKeyFrame(int _trackIndex, UINT _key);
	int GetTotalFrame() { return animTrackList[currentAnimTrackIndex].animList[0].size(); }
public:
	const std::vector<AnimList>& GetAnimTrackList() const { return animTrackList; }
};

