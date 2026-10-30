#pragma once

#include "character.h"
#include "Graphics/skinned_mesh.h"
#include "animator.h"
#include <memory>
#include <nlohmann/json.hpp>
#include "Ai/BehaviorTree/behavior_tree.h"
#include "Ai/BehaviorTree/Blackboard/blackboard.h"

#define EPSILON 1e-6f

class Player;

class Sandbag : public Character
{
public:
	Sandbag(ID3D11Device* device);
	~Sandbag();

	Sandbag(const Sandbag&) = delete;
	Sandbag& operator=(const Sandbag&) = delete;

	static Sandbag& Instance(ID3D11Device* device)
	{
		static Sandbag instance(device);
		return instance;
	}

	void Update(float elapsed_time);

	void Render(ID3D11DeviceContext* device_context);

	void DebugRenderGui();

	void SetPlayer(Player* player) { this->player = player; }

protected:
	void OnDamaged() override;

private:
	void UpdateStateMachine(float elapsed_time);

	void CollisionProjectilesVsPlayer(); // 自身の弾とプレイヤー

	std::unique_ptr<SkinnedMesh>	mesh;
	std::unique_ptr<Animator>	animator;
	
	float	move_speed = 5.0f;
	float	turn_speed = DirectX::XMConvertToRadians(720);
	float	jump_speed = 5.0f;

	int		max_hp = 10;

	bool use_behavior_tree = false;

	nlohmann::json json_data;

	// 仮置き
	//BehaviorTree behavior_tree;
	std::unique_ptr<BehaviorTree> behavior_tree;
	Blackboard black_board;
	BTContext context;
	void BuildBehaviorTree();
	Player* player = nullptr;

	enum class StateId
	{
		None = -1,
		Idle,
		Damage,
		Death,
		Revive,
		ChargeAttack,
		ShotAttack,
		Escape,

		EnumCount
	};
	void SetState(StateId state_id);

public:
	class State
	{
	public:
		State(Sandbag* owner) :owner(owner) {}
		virtual ~State() = default;

	public:
		virtual void OnEnter() {}
		virtual void OnExit() {}
		virtual void OnUpdate(float elapsed_time) {}

	protected:
		Sandbag* owner;
	};

	class IdleState : public State
	{
	public:
		IdleState(Sandbag* owner) : State(owner) {}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class DamageState : public State
	{
	public:
		DamageState(Sandbag* owner) : State(owner) {}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class DeathState : public State
	{
	public:
		DeathState(Sandbag* owner):State(owner){}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class ReviveState : public State
	{
	public:
		ReviveState(Sandbag* owner):State(owner){}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class ChargeAttackState : public State
	{
	public:
		ChargeAttackState(Sandbag* owner):State(owner){}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;

	private:
		enum class Phase
		{
			Ready,
			Active,
			Recovery
		};

		Phase phase = Phase::Ready;
		float timer{};

		DirectX::XMFLOAT3 dash_start_position{};
		DirectX::XMFLOAT3 dash_direction{};

		DirectX::XMFLOAT3 target{};
	};

	class ShotAttackState : public State
	{
	public:
		ShotAttackState(Sandbag* owner):State(owner){}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;

	private:
		float timer{};
		float shot_timer {};
		float shot_interval = 0.1f;
		float shot_speed = 30.0f;
		float shot_scale = 5.0f;
		DirectX::XMFLOAT3 target{};
	};

	class EscapeState : public State
	{
	public:
		EscapeState(Sandbag* owner):State(owner){}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;

	private:
		enum class Phase
		{
			Ready,
			Active,
			Recovery
		};

		Phase phase = Phase::Ready;

		float base_dist = 40.0f;

		DirectX::XMFLOAT3 target{};
	};

	StateId current_state = StateId::None;
	StateId next_state = StateId::None;
	std::unique_ptr<State> states[static_cast<size_t>(StateId::EnumCount)];
};