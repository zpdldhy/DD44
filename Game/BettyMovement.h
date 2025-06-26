#pragma once
#include "UScriptComponent.h"
#include "BettyStates.h"

class AActor;
class TEnemy;

class BettyMovement : public UScriptComponent
{
public:
	BettyMovement() = default;
	virtual ~BettyMovement() = default;
private:
	weak_ptr<AActor> player;
private:
	// Phase
	enum BettyAction { Intro = 0, Attack, Die };
	BettyAction currentAction = Intro;
	//// Common
	float hitElapsed = 1.0f;
	Vec3 distance;
	Vec3 nearOffset = { 5.0f, 0.0f, 5.0f };
	weak_ptr<UMeshComponent> leftHand;
	weak_ptr<UMeshComponent> rightHand;

	shared_ptr<AActor> leftRange;
	shared_ptr<AActor> rightRange;
	shared_ptr<AActor> bodyRange;

	bool canMeleeAttack = true;
	float attackElapsed = 0.0f;
	float attackOffset = 2.0f;
	int meleeAttackCount = 3;

	//// Roll
	int rollCount = 3;
	//// DropSnow
	vector<shared_ptr<AActor>> snowList;
	vector<shared_ptr<AActor>> activeSnowList;
	float snowElapsed = 0.0f;
	float snowOffset = 3.0f;
	float snowDropElapsed = 0.0f;
	float snowDropInterval = 0.2f;
	int snowDropCount = 0;
	int currentSnowDropCount = 0;
	bool bDropping;
	bool bSnowControl = false;

	// FX
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;

	// States 
	shared_ptr<BettyStateBase> idle;
	shared_ptr<BettyStateBase> intro;
	shared_ptr<BettyStateBase> jumpAttack;
	shared_ptr<BettyStateBase> twoHandAttack;
	shared_ptr<BettyStateBase> oneHandBackAttack;
	shared_ptr<BettyStateBase> oneHandDownAttack;
	shared_ptr<BettyStateBase> rollAttack;
	shared_ptr<BettyStateBase> dropAttack;
	shared_ptr<BettyStateBase> roarAttack;
	shared_ptr<BettyStateBase> death;
	shared_ptr<BettyStateBase> currentState;
	UINT currentStateId;
	vector<shared_ptr<BettyStateBase>> meleeState;
	vector<shared_ptr<BettyStateBase>> rangedState;
	int meleeIndex = 0;
	int rangedIndex = 0;

	bool bCanStart2 = true;
	bool bCanStart3 = true;

public:
	void Init();
	void Tick();
	virtual shared_ptr<UScriptComponent> Clone() override;
public:
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
	void SetSnowBall();
	void SetAttackRange();
	shared_ptr<AActor> CreateSnowBall();
	void ChangeState(shared_ptr<BettyStateBase> _state);


	// GetBettyAction
	bool IsBettyDie() { return currentAction == BettyAction::Die; }
public:
	//Phase
	void HandleAttack(float _deltaTime);
	//// 2,3
	void DropSnowBall(float _deltaTime);
	shared_ptr<AActor> GetSnowBall();
	void HandleSnowBall();
public:
	//Collision
	void CheckHit();
	void AddColliderActor();
public:
	//FX
	void Flashing();
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value);
};

