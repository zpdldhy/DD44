#pragma once
#include <unordered_set>

struct AnimList
{
	vector<vector<Matrix>> animList;
	wstring m_szName;
};

struct CbAnimData
{
	Matrix boneAnim[250];
};

struct AnimEventHash {
	size_t operator()(const pair<int, UINT>& p) const {
		size_t h1 = hash<int>()(p.first);
		size_t h2 = hash<UINT>()(p.second);
		return h1 ^ (h2 << 1);
	}
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
	unordered_map<pair<int, UINT>, function<void()>, AnimEventHash> eventMap;

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
	void AddEvent(int _trackIndex, UINT _keyFrame, function<void()> _func);
	void TriggetEvent(int _trackIndex, UINT currentFrame);
public:
	const std::vector<AnimList>& GetAnimTrackList() const { return animTrackList; }
};

