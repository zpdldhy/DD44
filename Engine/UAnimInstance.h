#pragma once
#include <unordered_set>
#include "AnimData.h"

struct AnimList
{
	wstring m_szName;
	int startFrame;
	int endFrame;
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
	shared_ptr<AnimData> m_pData;

	wstring m_modelName;
	vector<AnimList> animTrackList;

	float animFrame = 0.0f;
	UINT currentAnimTrackIndex = 0;
	UINT lastEventFrame = UINT_MAX;

	Vec3 rootPos = { 0, 0, 0 };
	Matrix rootMat;
	int rootIndex = 0;
	
	int prevIndex;
	UINT boneCount;

public:
	bool m_bOnPlayOnce;
	bool m_bInPlace = false;
	bool m_bPlay = true;
	float m_fAnimPlayRate = 30.0f;
	map<int, UINT> m_mKeyFrameMap;
	unordered_map<pair<int, UINT>, function<void()>, AnimEventHash> eventMap;

public:
	void PlayOnce(int _index);
	void SetCurrentAnimTrack(int _index);
public:
	void Tick();
	void Render();
	shared_ptr<UAnimInstance> Clone();
public:
	// AnimMap 만들 때 단 한 번 실행
	void MakeAnimData(const vector<XMFLOAT4>& _data);
	
	void AddTrack(AnimList _animTrack);
	void SetName(wstring _name) { m_modelName = _name; }
	wstring GetName() { return m_modelName; }
	 //
	void CheckInPlace(bool _inPlace) { m_bInPlace = _inPlace; }
 	void SetRootIndex(int _index) { rootIndex = _index; }
	int GetRootIndex() { return rootIndex; }
	//
	int GetAnimIndex(wstring _animName);
	Matrix GetBoneAnim(int _boneIndex);
	int GetTotalFrame() { return animTrackList[currentAnimTrackIndex].endFrame; }
	
	int GetCurrentIndex() { return currentAnimTrackIndex; }
	int GetCurrentFrame() { return animFrame; }
	Vec3 GetRootPos() { return rootPos; }

	Matrix GetMatrix(int _track, int _bone, int _frame);
	// temp ?
	void SetBoneCount(UINT _bone);
public:
	void SetKeyFrame(int _trackIndex, UINT _key);
	void AddEvent(int _trackIndex, UINT _keyFrame, function<void()> _func);
	void DeleteEvent(int _trackIndex, UINT _keyFrame);
	void TriggetEvent(int _trackIndex, UINT currentFrame);
public:
	const std::vector<AnimList>& GetAnimTrackList() const { return animTrackList; }
};

