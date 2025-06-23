#pragma once

// 애니메이션 텍스쳐를 만들고 가지고 있는 클래스
// 한 모델의 AnimInstance들은 하나의 AnimData 공유함
class AnimData
{
public:
	AnimData() = default;
	~AnimData() = default;
private:
	// GPU용 데이터 저장 전용
	vector<XMFLOAT4> m_animTexData;
	ComPtr<ID3D11Texture3D> m_pTex3D;
	ComPtr<ID3D11ShaderResourceView> m_pTexSRV;

	int texWidth;
	int texHeight;
	int texDepth;
public:
	void AddData(const vector<XMFLOAT4>& _data);
	void CreateTex(int _boneCount, int _trackCount);
	void Bind();
public:
	Matrix GetMatrix(int _track, int _bone, int _frame);

};

