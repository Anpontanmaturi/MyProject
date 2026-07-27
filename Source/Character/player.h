#pragma once

#include "character.h"
#include "Graphics/skinned_mesh.h"
#include "animator.h"
#include "Camera/camera_controller.h"
#include <memory>

class Sandbag;

class Player :public Character
{
public:
	Player(ID3D11Device* device);
	~Player();

	void Update(float elapsed_time);

	void Render(ID3D11DeviceContext* device_context);

	void DebugRenderGUI();

	bool MoveOtherBack() const { return move_otherback; }

	DirectX::XMFLOAT3 GetCameraLookAt() const
	{
		float offset_y = (mesh->GetModelHeight() * scale.y) * 0.25f;
		return DirectX::XMFLOAT3(position.x, position.y + offset_y, position.z);
	}

	void SetTargetEnemy(Sandbag* target) { enemy_target = target; }

	// カメラモードをプレイヤーから見れるように
	void SetCameraMode(CameraMode mode) { camera_mode = mode; }

	// プレイヤーの有効無効を切り替える
	void SetEnable(bool enable) { this->enable = enable; }

protected:
	// 着地した時に呼ばれる
	void OnLanding() override;

private:
	void UpdateStateMachine(float elapsed_time);

	// チャージ用関数
	void UpdateCharge(float elaapsed_time);

	// スティックの入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec();

	bool InputMove(float elapsed_time);
	bool InputJump();
	bool InputAttack();
	bool InputDash();

	void CollisionProjectilesVsEnemys();

	std::unique_ptr<SkinnedMesh>	mesh;
	std::unique_ptr<Animator>	animator;
	
	float	speed = {};
	float	move_speed = 5.0f;
	float	dash_speed = 15.0f;
	float	turn_speed = DirectX::XMConvertToRadians(720);
	float	jump_speed = 5.0f;
	bool	move_otherback = false;

	ID3D11Device* p_device = nullptr;

	Sandbag* enemy_target = nullptr;
	CameraMode camera_mode = CameraMode::Normal;

	int jump_count = 1;
	int dash_count = 1;

	float charge_timer = 0.0f;
	const float full_charge_time = 2.0f;
	float flash_speed = 7.5f;

	bool enable = false;

	// 仮置き
	bool space_division = true;

	enum class StateId
	{
		None = -1,
		Idle,
		Move,
		Jump,
		Attack,
		Dash,
		WallSlide,

		EnumCount
	};
	void SetState(StateId state_id);

public:
	class State
	{
	public:
		State(Player* owner):owner(owner){}
		virtual ~State() = default;

	public:
		virtual void OnEnter(){}
		virtual void OnExit(){}
		virtual void OnUpdate(float elapsed_time){}

	protected:
		Player* owner;
	};

	class IdleState : public State
	{
	public:
		IdleState(Player* owner) : State(owner) {}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class MoveState : public State
	{
	public:
		MoveState(Player* owner) : State(owner) {}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class JumpState : public State
	{
	public:
		JumpState(Player* owner) : State(owner) {}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class AttackState :public State
	{
	public:
		AttackState(Player* owner):State(owner){}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class DashState : public State
	{
	public:
		DashState(Player* owner) :State(owner) {}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	class WallSlideState : public State
	{
	public:
		WallSlideState(Player* owner) : State(owner) {}
		void OnEnter() override;
		void OnUpdate(float elapsed_time) override;
	};

	StateId current_state = StateId::None;
	StateId next_state = StateId::None;
	std::unique_ptr<State> states[static_cast<size_t>(StateId::EnumCount)];
};
