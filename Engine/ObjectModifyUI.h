#pragma once
class UMeshComponent;
class USkinnedMeshComponent;
class UStaticMeshComponent;
class UAnimInstance;
class UMaterial;
class Texture;

enum class EMenuTab { Actor, Asset, Texture, Mesh };

class ObjectModifyUI
{
private: 
	vector<shared_ptr<UMeshComponent>> m_vMeshList;
    vector<shared_ptr<Texture>> m_vTextureList;
	vector<shared_ptr<UAnimInstance>> m_vAnimList;
    vector<shared_ptr<UMaterial>> m_vStaticMatList;
    vector<shared_ptr<UMaterial>> m_vSkinnedMatList;

    EMenuTab currentTab = EMenuTab::Actor;
    bool m_bEditMesh;
public:
    void DrawUI();
    void DrawMeshUI();
    void SetOnCreateCallback(std::function<void(PrefabData)> callback)
    {
        m_OnCreate = std::move(callback);
    }
    void SetMeshList(vector<shared_ptr<UMeshComponent>> _meshList) { m_vMeshList = _meshList; }
    void SetTexList(vector<shared_ptr<Texture>> _texList) { m_vTextureList = _texList; }
    void SetAnimList(vector<shared_ptr<UAnimInstance>> _animList) { m_vAnimList = _animList; }
    void SetStaticMatList(vector<shared_ptr<UMaterial>> _matList) { m_vStaticMatList = _matList; }
    void SetSkinnedMatList(vector<shared_ptr<UMaterial>> _matList) { m_vSkinnedMatList = _matList; }

private:
    std::function<void(PrefabData)> m_OnCreate;
};

