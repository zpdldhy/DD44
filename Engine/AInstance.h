#pragma once
#include "AActor.h"
#include "EffectType.h"

class AInstance : public AActor
{
public:
	AInstance() = default;
	virtual ~AInstance() = default;

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;
	virtual void RenderShadow() override;
	virtual void Destroy() override;

public:
	void SetEffectType(EEffectType type) { m_eType = type; }
	EEffectType GetEffectType() const { return m_eType; }

	wstring GetMeshPath() { return m_vMeshList[0][0]->GetMeshPath(); }
	wstring GetTexturePath() { return m_vMeshList[0][0]->GetMaterial()->GetTexturePath(); }

	void SetInstanceMesh(shared_ptr<UMeshComponent> _pMeshCom);
	void AddInstanceMesh(shared_ptr<UMeshComponent> _pMeshCom);

private:
	void InsertChildsMesh(vector<shared_ptr<UMeshComponent>> _vChildsList);
	void InsertChildsMesh(vector<shared_ptr<UMeshComponent>> _vChildsList, UINT& _iMeshCount);

protected:
	// 1번째, Mesh 종류별 분류
	// 2번째, Actor당 분류(Transform, Color, UVList)
	vector<vector<shared_ptr<UMeshComponent>>> m_vMeshList;

	shared_ptr<Inputlayout> m_pInputLayout = nullptr;
	shared_ptr<Shader> m_pShader = nullptr;

private:
	EEffectType m_eType = EEffectType::Count;
};

