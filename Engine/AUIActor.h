#pragma once
#include "AActor.h"
#include "UIState.h"

class Texture;

class AUIActor : public AActor
{
public:
	AUIActor() { m_pState = make_shared<IdleUIState>(); m_eStateType = UIStateType::ST_IDLE; }
	virtual ~AUIActor() = default;

public:
	void Tick() override;

public:
	void SetState(shared_ptr<UIState> _pState) { m_pState = _pState; }
	void SetStateType(UIStateType _eStateType) { m_eStateType = _eStateType; }
	UIStateType GetStateType() { return m_eStateType; }

	void SetIdleTexture(shared_ptr<Texture> _pTexture) { m_pIdleTexture = _pTexture; }
	void SetHoverTexture(shared_ptr<Texture> _pTexture) { m_pHoverTexture = _pTexture; }
	void SetActiveTexture(shared_ptr<Texture> _pTexture) { m_pActiveTexture = _pTexture; }
	void SetSelectTexture(shared_ptr<Texture> _pTexture) { m_pSelectTexture = _pTexture; }

private:
	// State
	shared_ptr<UIState> m_pState = nullptr;
	UIStateType m_eStateType = UIStateType::ST_IDLE;

	// Texture
	shared_ptr<Texture> m_pIdleTexture = nullptr;
	shared_ptr<Texture> m_pHoverTexture = nullptr;
	shared_ptr<Texture> m_pActiveTexture = nullptr;
	shared_ptr<Texture> m_pSelectTexture = nullptr;	
};

