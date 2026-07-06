#pragma once

#include "character.h"
#include "Graphics/skinned_mesh.h"
#include "animator.h"
#include <memory>

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

protected:
	void OnDamaged() override;

private:
	void UpdateStateMachine(float elapsed_time);

	std::unique_ptr<SkinnedMesh>	mesh;
	std::unique_ptr<Animator>	animator;
	
	float	move_speed = 5.0f;
	float	turn_speed = DirectX::XMConvertToRadians(720);
	float	jump_speed = 5.0f;

	enum class StateId
	{
		None = -1,
		Idle,
		Damage,

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

	StateId current_state = StateId::None;
	StateId next_state = StateId::None;
	std::unique_ptr<State> states[static_cast<size_t>(StateId::EnumCount)];
};