#pragma once
#include <functional>

enum class EMenuTab { Asset, Texture, Mesh };

class ObjectEditorUI
{
public:
	void InitData(int _max, int _meshCount)
	{
		objectMaxNum = _max;
		meshNum = _meshCount;
	}

	void Update();

	void SetTargetObjectNumByCallback(std::function<void(int)> callback)
	{
		m_OnTargetNumApply = std::move(callback);
	}

	void SetMeshCallback(std::function<void(int, int)> callback)
	{
		m_OnAddMeshApply = std::move(callback);
	}

	int objectMaxNum = 0;
	int meshNum = 0;
private:
	EMenuTab currentTab = EMenuTab::Texture;
	
	int targetNum = 0;
	std::function<void(int)> m_OnTargetNumApply;
	std::function<void(int, int)> m_OnAddMeshApply;

};

