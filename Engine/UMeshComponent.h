#pragma once
#include "UPrimitiveComponent.h"
#include "UMeshResources.h"

class UMeshComponent : public UPrimitiveComponent, enable_shared_from_this<UMeshComponent>
{
public:
	UMeshComponent() = default;
	virtual ~UMeshComponent() = default;

protected:
	std::wstring m_MeshPath;
	vector<shared_ptr<UMeshComponent>>	m_vChild;
	shared_ptr<UMeshResources> m_pMesh = nullptr;
	bool bRender = true;
	bool bUseInstance = false;

public:
	void Render() override;

public:
	void SetMeshPath(const std::wstring& path) { m_MeshPath = path; }
	const std::wstring& GetMeshPath() const { return m_MeshPath; }

	virtual void SetMesh(shared_ptr<UMeshResources> _mesh) { m_pMesh = _mesh; }
	const shared_ptr<UMeshResources>& GetMesh() { return m_pMesh; }
	int GetType() { return m_pMesh->GetType(); }

	// child
	void AddChild(shared_ptr<UMeshComponent> _child){ m_vChild.push_back(_child); }
	void RemoveChild(int _index);
	bool RemoveChild(const wstring& _name);
	int GetChildIndex(shared_ptr<UMeshComponent> _child);
	shared_ptr<UMeshComponent> GetChild(int _index) { return m_vChild[_index]; }
	shared_ptr<UMeshComponent> GetChildByName(const wstring& _name);
	vector<shared_ptr<UMeshComponent>> GetChildren() { return m_vChild; }
	int GetChildCount() const { return static_cast<int>(m_vChild.size()); }
public:
	void SetVisible(bool _visible) { bRender = _visible; }
	const bool& GetVisible() { return bRender; }
	void SetUseInstance(bool _bUse) { bUseInstance = _bUse; }
	bool IsUseInstance() { return bUseInstance; }
};

