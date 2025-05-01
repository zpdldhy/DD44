#include "UMeshComponent.h"
#include "USkeletalMeshResources.h"

class UAnimation;

class USkinnedMeshComponent : public UMeshComponent
{
public:
	USkinnedMeshComponent() = default;
	virtual ~USkinnedMeshComponent() = default;

protected:
	shared_ptr<USkeletalMeshResources> m_pMesh = nullptr;
	shared_ptr<UAnimation> m_pAnim;


public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void PostRender() override;
	void Destroy() override;

public:
	void SetAnim(shared_ptr<UAnimation> _anim) { m_pAnim = _anim; }
	void SetMesh(shared_ptr<USkeletalMeshResources> _mesh) { m_pMesh = _mesh; }
	shared_ptr<USkeletalMeshResources> GetMesh() { return m_pMesh; }
};
