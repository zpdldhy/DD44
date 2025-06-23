#include "pch.h"
#include "ProjectileManager.h"

#include "AActor.h"
#include "TCharacter.h"
#include "UStaticMeshComponent.h"
#include "UBoxComponent.h"
#include "USphereComponent.h"
#include "ObjectManager.h"
#include "EnemyCollisionManager.h"
#include "Timer.h"
#include "EffectManager.h"

#include "MeshLoader.h"


void ProjectileManager::Init()
{
	// Create Pool
	Create();
}

void ProjectileManager::Tick()
{
	// ActiveList 이동 관리
	for (auto iter = activeObjList.begin(); iter != activeObjList.end(); )
	{
		auto proj = (*iter);

		(*iter).elapsed += TIMER->GetDeltaTime();

		//// 너무 오래 계셨어요
		//if ((*iter).elapsed > (*iter).maxTime)
		//{
		//	DeactivateOne(*iter);
		//	iter = activeObjList.erase(iter);
		//	continue;
		//}


		Vec3 dir = ((*iter).dir);
		//dir.x *= (*iter).velocity;
		//dir.y *= (*iter).velocity;
		//dir.z *= (*iter).heightRatio;
		dir.y *= 0.7f;

		(*iter).projectile->AddPosition(dir);

		m_fMagicSpawnTimer += TIMER->GetDeltaTime();
		// Effect
		//if (m_fMagicSpawnTimer >= m_fMagicSpawnDelay)
		{
			if ((*iter).type == ProjectileType::MagicBall)
			{
				proj.projectile->GetMeshComponent()->SetInstanceColor(Vec4(1, 0, 0, 1));
				EFFECT->PlayEffect(EEffectType::Point, (*iter).projectile->GetPosition(), 0, Vec3(0, 0, 0));
			}
			m_fMagicSpawnTimer -= m_fMagicSpawnDelay;
		}
		



		// 어딘가에 충돌
		// 충돌이 안됨 ..  
		if ((*iter).projectile->m_vCollisionList.size() > 0)
		{
			auto list = (*iter).projectile->m_vCollisionList;
			for (auto& index : list)
			{
				auto actor = OBJECT->GetActor(index.first);
				auto healthComp = dynamic_pointer_cast<TCharacter>(actor);
				if (healthComp)
				{
					healthComp->CheckHitByProjectile((int)(*iter).type, true);
				}
			}

			DeactivateOne(*iter);
			iter = activeObjList.erase(iter);
			continue;
		}
		iter++;
	}

}

void ProjectileManager::Destroy()
{
	arrowList.clear();
	magicList.clear();
	activeObjList.clear();
}

void ProjectileManager::Create()
{
	arrowList.resize(4);
	for (int i = 0; i < 4; i++)
	{
		auto projectile = make_shared<AActor>();
		auto mesh = MESHLOADER->Make("../Resources/Asset/Arrow.mesh.json");
		projectile->SetMeshComponent(mesh);
		auto collider = make_shared<UBoxComponent>();
		collider->SetName(L"PlayerAttack");
		collider->SetLocalScale(Vec3(0.5f, 0.5f, 3.0f));
		collider->SetLocalPosition(Vec3(0.0f, 0.0f, 1.5f));
		collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
		projectile->SetShapeComponent(collider);
		projectile->m_szName = L"PlayerAttack";
		projectile->m_bCastShadow = false;

		OBJECT->AddActor(projectile);
		ENEMYCOLLIDER->Add(projectile);

		projectile->m_bCollision = false;
		collider->m_bVisible = false;
		mesh->SetVisible(false);

		Vec3 identity;
		ProjectileData data;
		data.projectile = projectile;
		data.InitData(ProjectileType::PlayerArrow, identity, identity);
		arrowList[i] = data;
	}

	magicList.resize(10);
	for (int i = 0; i < 10; i++)
	{
		auto ball = make_shared<AActor>();
		auto mesh = MESHLOADER->Make("../Resources/Asset/Sphere.mesh.json");
		ball->SetMeshComponent(mesh);
		auto collider = make_shared<USphereComponent>();
		collider->SetName(L"Enemy");
		collider->SetLocalScale(Vec3(0.5f, 0.5f, 0.5f));
		collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
		ball->SetShapeComponent(collider);
		ball->m_szName = L"MageSphere";
		ball->m_bCastShadow = false;

		OBJECT->AddActor(ball);
		ENEMYCOLLIDER->Add(ball);

		ball->m_bCollision = false;
		
		collider->m_bVisible = false;
		mesh->SetVisible(false);

		Vec3 identity;
		ProjectileData data;
		data.projectile = ball;

		data.InitData(ProjectileType::MagicBall, identity, identity);
		magicList[i] = data;
	}
}

void ProjectileManager::ActivateOne(ProjectileType _type, Vec3 _pos, Vec3 _dir)
{
	switch (_type)
	{
	case ProjectileType::PlayerArrow: {
		for (auto& arrow : arrowList)
		{
			if (arrow.projectile->m_bCollision == false)
			{
				// use this
				arrow.startPos = _pos;
				arrow.dir = _dir;
				arrow.elapsed = 0.0f;

				arrow.projectile->m_bCollision = true;
				arrow.projectile->GetShapeComponent()->m_bVisible = true;
				arrow.projectile->GetMeshComponent()->SetVisible(true);
				// R
				float targetYaw = atan2f(_dir.x, _dir.z);
				Vec3 currentRot = arrow.projectile->GetRotation();
				currentRot.y = targetYaw;
				arrow.projectile->SetRotation(currentRot);

				// T
				arrow.projectile->SetPosition(_pos);

				activeObjList.push_back(arrow);
				break;
			}
		}
		// temp
		// Add other
		break;
	}
	case ProjectileType::MagicBall: {
		for (auto& ball : magicList)
		{
			if (ball.projectile->m_bCollision == false)
			{
				// use this
				ball.startPos = _pos;
				ball.dir = _dir;
				ball.elapsed = 0.0f;

				ball.projectile->m_bCollision = true;
				ball.projectile->GetShapeComponent()->m_bVisible = true;
				ball.projectile->GetMeshComponent()->SetVisible(true);

				// T
				ball.projectile->SetPosition(_pos);

				activeObjList.push_back(ball);


				break;
			}
		}
		// temp
		// Add other
		break;
	}
	default:
		break;
	}
}

void ProjectileManager::DeactivateOne(ProjectileData _target)
{
	_target.projectile->m_bCollision = false;
	_target.projectile->GetShapeComponent()->m_bVisible = false;
	_target.projectile->GetMeshComponent()->SetVisible(false);

}

vector<shared_ptr<AActor>> ProjectileManager::GetActorList()
{
	// 매 프레임 되니까 괜찮나 ? 
	vector<shared_ptr<AActor>> colList;
	for (auto& obj : activeObjList)
	{
		colList.push_back(obj.projectile);
	}
	return colList;
}
