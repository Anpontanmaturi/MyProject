#pragma once

#include "Graphics/skinned_mesh.h"
#include "animator.h"

class Player
{
public:
	Player(ID3D11Device* device);

	void Update(float elapsed_time);

	void Render(ID3D11DeviceContext* device_context);

	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	void SetRotation(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }
	const DirectX::XMFLOAT3& GetRotation() const { return rotation; }

	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	void SetColor(const DirectX::XMFLOAT4& color) { this->color = color; }
	const DirectX::XMFLOAT4& GetColor() const { return color; }

	bool IsGround() const {return is_ground;}
	bool MoveOtherBack() const { return move_otherback; }

	DirectX::XMFLOAT3 GetCameraLookAt() const
	{
		float offset_y = (mesh->GetModelHeight() * scale.y) * 0.25f;
		return DirectX::XMFLOAT3(position.x, position.y + offset_y, position.z);
	}

private:
	void UpdateTransform();
	void UpdateVelocity(float elapsed_time);
	void UpdateStateMachine(float elapsed_time);

	bool InputMove();
	bool InputJump();
	bool InputAttack();

	std::unique_ptr<SkinnedMesh>	mesh;
	std::unique_ptr<Animator>	animator;
	DirectX::XMFLOAT3	position = {};
	DirectX::XMFLOAT3	rotation = {};
	DirectX::XMFLOAT3	scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4	transform =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	DirectX::XMFLOAT4	color = { 1, 1, 1, 1 };

	DirectX::XMFLOAT3	velocity = {};
	float	gravity = 10.0f;
	float	acceleration = 50.0f;
	float	deceleration = 20.0f;
	float	move_speed = 5.0f;
	float	turn_speed = DirectX::XMConvertToRadians(720);
	float	jump_speed = 5.0f;
	float	air_control = 0.3f;
	float	input_move_x = 0.0f;
	float	input_move_z = 0.0f;
	bool	is_ground = false;
	bool	move_otherback = false;

	enum class StateId
	{
		None = -1,
		Idle,
		Move,
		Jump,
		Attack,

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

	StateId current_state = StateId::None;
	StateId next_state = StateId::None;
	std::unique_ptr<State> states[static_cast<size_t>(StateId::EnumCount)];
};
