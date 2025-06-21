#pragma once

// �ִϸ��̼� �ؽ��ĸ� ����� ������ �ִ� Ŭ����
// �� ���� AnimInstance���� �ϳ��� AnimData ������
class AnimData
{
public:
	AnimData() = default;
	~AnimData() = default;
private:
	// GPU�� ������ ���� ����
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

