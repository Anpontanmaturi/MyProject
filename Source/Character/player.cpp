#include "player.h"
#include "Camera/camera.h"
#include "Input/game_pad.h"
#include "Collision/collision_manager.h"

Player::Player(ID3D11Device* device)
{
	const char* filename = ".\\Data\\Model\\Jammo\\Jammo.fbx";
	mesh = std::make_unique<SkinnedMesh>(device, filename, true, 0.0f, axis_system::rhs_y_up);
	animator = std::make_unique<Animator>(mesh.get());

	// ステート生成
	states[static_cast<size_t>(StateId::Idle)] = std::make_unique<IdleState>(this);
	states[static_cast<size_t>(StateId::Move)] = std::make_unique<MoveState>(this);
	states[static_cast<size_t>(StateId::Jump)] = std::make_unique<JumpState>(this);

	SetState(StateId::Idle);
}

void Player::Update(float elapsed_time)
{
	UpdateStateMachine(elapsed_time);

	UpdateVelocity(elapsed_time);

	animator->Update(elapsed_time);

	UpdateTransform();
}

void Player::Render(ID3D11DeviceContext* device_context)
{
	mesh->Render(device_context, transform, color, animator->GetCurrentKeyframe());
}

void Player::UpdateVelocity(float elapsed_time)
{
	// 加速処理
	float input_move_length = sqrtf(input_move_x * input_move_x + input_move_z * input_move_z);
	if(input_move_length > 0)
	{
		float vec_x = input_move_x / input_move_length;
		float vec_z = input_move_z / input_move_length;

		float acceleration = this->acceleration * elapsed_time;
		if (!is_ground) acceleration *= air_control;

		velocity.x += vec_x * acceleration;
		velocity.z += vec_z * acceleration;

		// 最大速度制限
		float velocity_length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
		if (velocity_length > move_speed)
		{
			velocity.x = (velocity.x / velocity_length) * move_speed;
			velocity.z = (velocity.z / velocity_length) * move_speed;
		}

		// 進行方向に向ける
		{
			// 向いている方向
			float front_x = sinf(rotation.y);
			float front_z = cosf(rotation.y);

			// 回転量調整
			float dot = front_x * vec_x + front_z * vec_z;
			float rot = (std::min)(1.0f - dot, turn_speed * elapsed_time);

			// 左右判定をして回転処理
		}
	}
}