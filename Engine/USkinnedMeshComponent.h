#include "UMeshComponent.h"
#include "USkeletalMeshResources.h"

class AnimTrack;
class UAnimInstance;

class USkinnedMeshComponent : public UMeshComponent
{
public:
	USkinnedMeshComponent() = default;
	virtual ~USkinnedMeshComponent() = default;

protected:
	shared_ptr<USkeletalMeshResources> m_pMesh = nullptr;
	shared_ptr<UAnimInstance> m_pBaseAnim;
	shared_ptr<AnimTrack> m_pMeshAnim;

public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void PostRender() override;
	void Destroy() override;

public:
	void SetBaseAnim(shared_ptr<UAnimInstance> _anim) { m_pBaseAnim = _anim; }
	void SetMeshAnim(shared_ptr<AnimTrack> _meshAnim) { m_pMeshAnim = _meshAnim; }
	void SetMesh(shared_ptr<USkeletalMeshResources> _mesh) { m_pMesh = _mesh; }
	shared_ptr<USkeletalMeshResources> GetMesh() { return m_pMesh; }
	shared_ptr<AnimTrack> GetAnim() { return m_pMeshAnim; }
	shared_ptr <UAnimInstance> GetAnimInstance() { return m_pBaseAnim; }
};
