#pragma once

struct CbAnimData
{
	Matrix boneAnim[250];
};

struct AnimList
{
	vector<vector<Matrix>> animList;
	wstring m_szName;
};

class UAnimation
{
public:
	CbAnimData currentAnimList;
	vector<AnimList> animTrackList;
	vector<Matrix> inversedBone;
	float animFrame = 0.0f;
	UINT currentAnimTrackIndex = 0;
	ComPtr<ID3D11Buffer> _constantBuffer;
public:
	void CreateConstantBuffer();
	void Tick();
	void Render();

};
