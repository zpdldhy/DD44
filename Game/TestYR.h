#pragma once
#include "IExecute.h"
#include "FbxLoader.h"

class APawn;

struct CbAnimData
{
	Matrix boneAnim[250];
};

class TestYR : public IExecute
{
public:
	FbxLoader m_FbxImporter;
	shared_ptr<APawn> m_pActor;
	shared_ptr<APawn> m_pActor2;

	shared_ptr<class ACameraActor> m_pCameraActor;


	// TEMP
	ComPtr<ID3D11Buffer> _constantBuffer;



public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
public:
	void MakeObjectByAsset(TFbxResource _resource);
};

