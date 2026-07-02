#pragma once

#include "Graphics/skinned_mesh.h"
#include "animator.h"
#include <memory>

class Sandbag
{
public:
	Sandbag(ID3D11Device* device);
	~Sandbag();

	static Sandbag& Instance(ID3D11Device* device)
	{
		static Sandbag instance(device);
		return instance;
	}

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

	float GetRadius() const { return radius; }
	float GetHeight() const { return height; }

	bool IsGround() const { return is_ground; }

private:
	void UpdateTransform();
	void UpdateVelocity(float elapsed_time);
	void UpdateStateMachine(float elapsed_time);

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

	float	radius = 1.0f / 28.0f;
	float	height = 2.0f / 28.0f;

	DirectX::XMFLOAT3	velocity = {};
	float	gravity = 10.0f;
	float	acceleration = 50.0f;
	float	deceleration = 20.0f;
	float	move_speed = 5.0f;
	float	turn_speed = DirectX::XMConvertToRadians(720);
	float	jump_speed = 5.0f;
	float	air_control = 0.3f;
	bool	is_ground = false;

	enum class StateId
	{
		None = -1,
		Idle,

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

	StateId current_state = StateId::None;
	StateId next_state = StateId::None;
	std::unique_ptr<State> states[static_cast<size_t>(StateId::EnumCount)];
};