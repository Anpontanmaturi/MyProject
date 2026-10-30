#include "sandbag.h"
#include "Collision/collision_manager.h"
#include "Graphics/graphics.h"
#include "player.h"

#include "Weapon/projectile_straight.h"
#include "projectile_manager.h"

#include "Ai/BehaviorTree/Composite/sequence_node.h"
#include "Ai/BehaviorTree/Composite/selector_node.h"
#include "Ai/BehaviorTree/Action/wait_node.h"
#include "Ai/BehaviorTree/Action/move_to_node.h"
#include "Ai/BehaviorTree/Action/dist_condition_node.h"

#include "Ai/BehaviorTree/behavior_tree_fact.h"
#include "Ai/BehaviorTree/bt_debug_gui.h"

#include <fstream>
#include <string>
#include <Windows.h>

namespace
{
	BlackboardKey PlayerPositionKey
	{
		"PlayerPosition",
		BlackboardValueType::Float3
	};
}

Sandbag::~Sandbag() = default;

Sandbag::Sandbag(ID3D11Device* device)
{
	const char* filename = ".\\Data\\Model\\Jammo\\Jammo.fbx";
	mesh = std::make_unique<SkinnedMesh>(device, filename, true, 0.0f, axis_system::rhs_y_up);
	animator = std::make_unique<Animator>(mesh.get());
	this->device = device;

	black_board.RegisterKey(PlayerPositionKey);

	BuildBehaviorTree();

	context.owner = this;
	context.blackboard = &black_board;

	this->SetHitPoint(max_hp);

	// 仮置き
	// ステート生成 
	states[static_cast<size_t>(StateId::Idle)] = std::make_unique<IdleState>(this);
	states[static_cast<size_t>(StateId::Damage)] = std::make_unique<DamageState>(this);
	states[static_cast<size_t>(StateId::Death)] = std::make_unique<DeathState>(this);
	states[static_cast<size_t>(StateId::Revive)] = std::make_unique<ReviveState>(this);
	states[static_cast<size_t>(StateId::Escape)] = std::make_unique<EscapeState>(this);

	// 攻撃系
	states[static_cast<size_t>(StateId::ChargeAttack)] = std::make_unique<ChargeAttackState>(this);
	states[static_cast<size_t>(StateId::ShotAttack)] = std::make_unique<ShotAttackState>(this);

	SetState(StateId::Idle);
}

void Sandbag::Update(float elapsed_time)
{
	black_board.SetValue(
		PlayerPositionKey,
		player->GetPosition());

	context.move_speed = move_speed;
	context.turn_speed = turn_speed;

	if (use_behavior_tree)
	{
		behavior_tree->Tick(context, elapsed_time);
	}
	else
	{
		UpdateStateMachine(elapsed_time);
	}

	UpdateVelocity(elapsed_time);

	UpdateInvincibleTimer(elapsed_time);

	CollisionProjectilesVsPlayer();

	animator->Update(elapsed_time);

	UpdateTransform();
}

void Sandbag::Render(ID3D11DeviceContext* device_context)
{
	mesh->Render(device_context, transform, color, animator->GetCurrentKeyframe());
}

void Sandbag::DebugRenderGui()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(1, 0, 0, 1));

	ImGui::Begin("Enemy(sandbag)");
	{
		ImGui::InputFloat3("position", &position.x);
		ImGui::InputFloat3("rotation", &rotation.x);
		ImGui::InputFloat4("color", &static_cast<float>(color.x));
		ImGui::Text("HP: %d", GetHitPoint());
		ImGui::Checkbox("use_behavior_tree", &use_behavior_tree);
		ImGui::Separator();
		if (ImGui::Button("Charge Attack")) { SetState(Sandbag::StateId::ChargeAttack); }
		if (ImGui::Button("Shot Attack")) { SetState(Sandbag::StateId::ShotAttack); }
		if (ImGui::Button("Escape")) { SetState(Sandbag::StateId::Escape); }
	}
	ImGui::End();
	if (behavior_tree && use_behavior_tree) {
		DrawBTDebugGui(json_data, behavior_tree->GetDebugInfo());
	}
}

void Sandbag::OnDamaged()
{
	if (is_invincible_state) return;

	--hp;

	if (hp <= 0)
	{
		SetState(StateId::Death);
		return;
	}

	SetState(StateId::Damage);
}

void Sandbag::CollisionProjectilesVsPlayer()
{
	int projectile_count = ProjectileManager::Instance().GetProjectileCount();
	for (int i = 0; i < projectile_count; i++)
	{
		Projectile* projectile = ProjectileManager::Instance().GetProjectile(i);

		if (projectile->GetOwner() != this) continue;

		projectile->SetScale(0.005f);
		projectile->SetSpeed(50.0f);

		DirectX::XMFLOAT3 out_position;
		if (CollisionManager::SphereVsCylinder(
			projectile->GetPosition(),
			projectile->GetRadius(),
			player->GetPosition(),
			player->GetRadius(),
			player->GetHeight(),
			out_position))
		{
			if (player->TakeDamege(1, 0.1f))
			{
				// 弾丸破棄
				projectile->Destroy();
			}
		}
	}
}

void Sandbag::UpdateStateMachine(float elapsed_time)
{
	// ステートの切り替え
	if (next_state != StateId::None)
	{
		if (current_state != StateId::None)
		{
			states[static_cast<size_t>(current_state)]->OnExit();
		}
		current_state = next_state;
		next_state = StateId::None;
		if (current_state != StateId::None)
		{
			states[static_cast<size_t>(current_state)]->OnEnter();
		}
	}
	// ステートの更新
	if (current_state != StateId::None)
	{
		states[static_cast<size_t>(current_state)]->OnUpdate(elapsed_time);
	}
}

void Sandbag::SetState(StateId state_id)
{
	next_state = state_id;
}


void Sandbag::IdleState::OnEnter()
{
	owner->animator->Play("Idle", true);
}

void Sandbag::IdleState::OnUpdate(float elapsed_time)
{
	
}

void Sandbag::DamageState::OnEnter()
{
	owner->animator->Play("GetHit1", false);
}

void Sandbag::DamageState::OnUpdate(float elapsed_time)
{
	if (owner->animator->IsFinished())
	{
		owner->SetState(StateId::Idle);
	}
}

void Sandbag::DeathState::OnEnter()
{
	owner->animator->Play("Death", false);
}

void Sandbag::DeathState::OnUpdate(float elapsed_time)
{
	if (owner->animator->IsFinished())
	{
		owner->SetState(StateId::Revive);
	}
}

void Sandbag::ReviveState::OnEnter()
{
	owner->animator->Play("Revive", false);
}

void Sandbag::ReviveState::OnUpdate(float elapsed_time)
{
	if (owner->animator->IsFinished())
	{
		owner->SetHitPoint(owner->max_hp);
		owner->SetState(StateId::Idle);
	}
}

void Sandbag::EscapeState::OnEnter()
{
	phase = Phase::Ready;
	owner->is_invincible_state = true;
}

void Sandbag::EscapeState::OnUpdate(float elapsed_time)
{
	switch (phase)
	{
	case Phase::Ready: {
		DirectX::XMFLOAT3 center = {};
		DirectX::XMFLOAT3 pos = owner->GetPosition();

		// 中心->敵
		float dx = pos.x - center.x;
		float dz = pos.z - center.z;
		float len = sqrtf(dx * dx + dz * dz);

		if (len < EPSILON)
		{
			target = { center.x, pos.y, center.z + base_dist };
			phase = Phase::Active;
			owner->Jump(owner->jump_speed * 4.0f);
			break;
		}

		// 反対へ
		dx = -dx / len;
		dz = -dz / len;

		target = { center.x + dx * base_dist, pos.y, center.z + dz * base_dist };
		phase = Phase::Active;
		owner->Jump(owner->jump_speed * 4.0f);
		break;
	}
	case Phase::Active: {
		DirectX::XMFLOAT3 pos = owner->GetPosition();

		owner->MoveTo(target, owner->move_speed * base_dist, owner->turn_speed, elapsed_time);

		// 仮置き
		float dx = target.x - pos.x;
		float dz = target.z - pos.z;
		float distsq = dx * dx + dz * dz;

		if (distsq <= 1.0f)
		{
			owner->MoveStop();
			if (owner->is_ground)
			{
				phase = Phase::Recovery;
				break;
			}
		}
		break;
	}
	case Phase::Recovery: {
		owner->SetState(StateId::Idle);
		owner->is_invincible_state = false;
		break;
	}
	}
}

void Sandbag::ChargeAttackState::OnEnter()
{
	phase = Phase::Ready;
	owner->is_invincible_state = true;
	owner->SetColor({ 0.0f, 0.2f, 1.0f, 1.0f });
	timer = 0.0f;
}

void Sandbag::ChargeAttackState::OnUpdate(float elapsed_time)
{
	timer += elapsed_time;

	switch (phase)
	{
	case Phase::Ready: {
		DirectX::XMFLOAT3 pos{};
		DirectX::XMFLOAT3 player_pos{};

		if (owner->player != nullptr)
		{
			target = owner->player->GetPosition();

			owner->TurnTo(target, owner->turn_speed, elapsed_time);

			pos = owner->GetPosition();
			player_pos = owner->player->GetPosition();
		}

		if (timer > 0.5f)
		{
			phase = Phase::Active;
			timer = 0.0f;
			float dx = player_pos.x - pos.x;
			float dz = player_pos.z - pos.z;
			float len = sqrtf(dx * dx + dz * dz);
			if (len > EPSILON) { dash_direction = { dx / len, 0.0f, dz / len }; }
			dash_start_position = pos;
		}
		break;
	}
	case Phase::Active: {
		DirectX::XMFLOAT3 pos = owner->GetPosition();
		DirectX::XMFLOAT3 player_pos = owner->player->GetPosition();

		// 仮置き
		// ここのあたり判定は後ほど細かいのに変える
		float dx = player_pos.x - pos.x;
		float dz = player_pos.z - pos.z;
		float distsq = dx * dx + dz * dz;

		if (timer >= 1.0f || distsq <= 0.1f)
		{
			phase = Phase::Recovery;
			timer = 0.0f;
			break;
		}

		owner->Move(dash_direction.x, dash_direction.z, owner->move_speed * 50.0f);
		break;
	}
	case Phase::Recovery: {
		owner->MoveStop();
		owner->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		owner->is_invincible_state = false;
		if (timer >= 1.0f)
		{
			owner->SetState(StateId::Idle);
		}
		break;
	}
	}
}

void Sandbag::ShotAttackState::OnEnter()
{
	timer = 0.0f;
	shot_timer = 0.0f;
}

void Sandbag::ShotAttackState::OnUpdate(float elapsed_time)
{
	timer += elapsed_time;
	shot_timer += elapsed_time;

	if (owner->player == nullptr) return;
	DirectX::XMFLOAT3 pos = owner->GetPosition();
	target = owner->player->GetPosition();

	if (timer < 3.0f)
	{
		if (shot_timer >= shot_interval)
		{
			shot_timer -= shot_interval;

			DirectX::XMFLOAT3 shot_pos = // 発射位置
			{
				pos.x, pos.y + ((owner->mesh->GetModelHeight() * owner->GetScale().y) * 0.15f), pos.z
			};

			target.y += 1.0f;
			// 向かう方向
			DirectX::XMFLOAT3 d = {
				target.x - shot_pos.x,
				target.y - shot_pos.y,
				target.z - shot_pos.z
			};
			float len = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
			if (len < EPSILON) return;

			DirectX::XMFLOAT3 direction = {
				d.x / len, d.y / len, d.z / len
			};

			auto* proj = new ProjectileStraight(&ProjectileManager::Instance(), owner->device);

			proj->SetOwner(owner);

			proj->Launch(direction, shot_pos);
		}
	}
	else
	{
		owner->SetState(StateId::Idle);
	}
}

void Sandbag::BuildBehaviorTree()
{
	/*auto root = std::make_unique<SequenceNode>();

	root->AddChild(std::make_unique<DistConditionNode>(
		PlayerPositionKey, DistanceCompare::Greater, 5.0f));

	root->AddChild(std::make_unique<MoveToNode>(PlayerPositionKey, move_speed, turn_speed, 0.2f));*/

	//root->AddChild(std::make_unique<WaitNode>(2.0f));

	/*auto sequence = std::make_unique<SequenceNode>();
	BlackboardKey key{
		"PlayerPosition",BlackboardValueType::Float3
	};
	auto condition = std::make_unique<DistConditionNode>(key, DistanceCompare::Greater, 5.0f);
	auto move_to = std::make_unique<MoveToNode>(key, move_speed, turn_speed, 0.2f);
	sequence->AddChild(std::move(condition));
	sequence->AddChild(std::move(move_to));
	behavior_tree.SetRoot(std::move(sequence));*/

	const char* file_path = ".\\Data\\BehaviorTree\\Sandbag.json";

	std::ifstream file(file_path);

	if (!file.is_open()) {
		OutputDebugStringA("Failed to open Data\\BehaviorTree\\Sandbag.json\n"); 
		return; 
	}

	try { file >> json_data; }
	catch (const nlohmann::json::exception& e)
	{
		std::string message = std::string("Failed to load JSON\n")+e.what();
		OutputDebugStringA(message.c_str());
		MessageBoxA(nullptr, message.c_str(), "BehaviorTree Error", MB_OK | MB_ICONERROR);
		return;
	}

	// BehaviorTreeを生成
	std::unique_ptr<BehaviorTree> tree = BehaviorTreeFact::CreateTree(json_data);

	if (!tree) { OutputDebugStringA("Failed to create BehaviorTree from Sandbug.json\n"); return; }

	behavior_tree = std::move(tree);
}

