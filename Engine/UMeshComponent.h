#pragma once
#include "UPrimitiveComponent.h"

class MeshTransform;

class UMeshComponent : public UPrimitiveComponent, enable_shared_from_this<UMeshComponent>
{
public:
	UMeshComponent() = default;
	virtual ~UMeshComponent() = default;

protected:
	std::wstring m_MeshPath;
	vector<shared_ptr<UMeshComponent>>	m_vChild;
	bool bRender = true;
	shared_ptr<MeshTransform> m_pMeshTrans;

public:
	void Render() override;

public:
	void SetMeshPath(const std::wstring& path) { m_MeshPath = path; }
	const std::wstring& GetMeshPath() const { return m_MeshPath; }

	void AddChild(shared_ptr<UMeshComponent> _child);// { m_vChild.push_back(_child); }
	void RemoveChild(int _index);
	void RemoveChild(const wstring& _name);
	int GetChildIndex(shared_ptr<UMeshComponent> _child);
	shared_ptr<UMeshComponent> GetChild(int _index) { return m_vChild[_index]; }
	vector<shared_ptr<UMeshComponent>> GetChildren() { return m_vChild; }
	shared_ptr<UMeshComponent> GetMeshByName(const wstring& _name);
	
	void SetMeshTransform(const shared_ptr<MeshTransform>& _pMeshTrans) { m_pMeshTrans = _pMeshTrans; }
	const shared_ptr<MeshTransform>& GetMeshTransform() { return m_pMeshTrans; }

	//// USceneComponent 말고 meshTransform의 함수 호출
	//virtual void SetLocalPosition(const Vec3& _pos) override;
	//virtual void SetLocalRotation(const Vec3& _rot) override; 
	//virtual void SetLocalScale(const Vec3& _scale) override; 
public:
	void SetVisible(bool _visible) { bRender = _visible; }
	const bool& GetVisible() {  return bRender; }

	//shared_ptr<UMeshComponent> Clone();

};

