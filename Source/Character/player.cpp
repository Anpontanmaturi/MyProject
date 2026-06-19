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
			float cross = front_x * vec_z - front_z * vec_x;
			if (cross < 0.0f)
			{
				rotation.y += rot;
			}
			else
			{
				rotation.y -= rot;
			}
		}
	}
	else
	{
		// 減速処理
		float deceleration = this->deceleration * elapsed_time;
		if (!is_ground) deceleration *= air_control;

		float velocityLength = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
		if (velocityLength > deceleration)
		{
			velocity.x -= (velocity.x / velocityLength) * deceleration;
			velocity.z -= (velocity.z / velocityLength) * deceleration;
		}
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	// 重力処理
	velocity.y -= gravity * elapsed_time;

	// 移動量
	float move_x = velocity.x * elapsed_time;
	float move_y = velocity.y * elapsed_time;
	float move_z = velocity.z * elapsed_time;

	// 水平移動処理
	float move_xz_length = sqrtf(move_x * move_x + move_z * move_z);
	if (move_xz_length > 0)
	{
		// レイの始点と終点を求める
		DirectX::XMFLOAT3 s = {
			position.x,
			position.y + 0.5f,
			position.z
		};
		DirectX::XMFLOAT3 e = {
			position.x + move_x,
			position.y + 0.5f,
			position.z + move_z
		};

		// ステージとレイキャストを行い、交点を法線を取得する
		HitResult hit_result;
		if (CollisionManager::Instance().Raycast(s, e, hit_result))
		{
			// 交点から終点へのベクトルを求める
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&hit_result.position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&e);
			DirectX::XMVECTOR PE = DirectX::XMVectorSubtract(E, P);

			// 三角関数で終点から壁までの長さを求める
			DirectX::XMVECTOR N = DirectX::XMLoadFloat3(&hit_result.normal);
			DirectX::XMVECTOR A = DirectX::XMVector3Dot(DirectX::XMVectorNegate(PE), N);
			// 壁までの長さを少しだけ長くなるように補正する
			float a = DirectX::XMVectorGetX(A) + 0.01f;

			// 壁ずりベクトルを求める
			DirectX::XMVECTOR R = DirectX::XMVectorAdd(PE, DirectX::XMVectorScale(N, a));

			// 壁ずり後の位置を求める
			DirectX::XMVECTOR Q = DirectX::XMVectorAdd(P, R);
			DirectX::XMFLOAT3 q;
			DirectX::XMStoreFloat3(&q, Q);

			// 壁際で壁ずり後の位置がめり込んでいないかレイキャストでチェックする
			if (CollisionManager::Instance().Raycast(s, q, hit_result))
			{
				// めり込んでいた場合はプレイヤーの位置に今回レイキャストした交点を設定する
				// ※プレイヤーの位置が壁にぴったりくっつかないように補正する
				P = DirectX::XMLoadFloat3(&hit_result.position);
				DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&s);
				DirectX::XMVECTOR PS = DirectX::XMVectorSubtract(S, P);
				DirectX::XMVECTOR V = DirectX::XMVector3Normalize(PS);
				P = DirectX::XMVectorAdd(P, DirectX::XMVectorScale(V, 0.001f));
				DirectX::XMFLOAT3 p;
				DirectX::XMStoreFloat3(&p, P);
				position.x = p.x;
				position.z = p.z;
			}
			else
			{
				// めり込んでいなかったのでプレイヤーの位置に壁ずり後の位置を設定する
				position.x = q.x;
				position.z = q.z;
			}
		}
		else
		{
			// 壁に当たらなかったので普通に移動
			position.x += move_x;
			position.z += move_z;
		}
	}

	// 上下移動処理
	DirectX::XMFLOAT3 start = { position.x, position.y + 1, position.z };
	DirectX::XMFLOAT3 end = { position.x, position.y + move_y, position.z };
	HitResult hit_result;
	if (CollisionManager::Instance().Raycast(start, end, hit_result))
	{
		position.y = hit_result.position.y;
		velocity.y = 0.0f;
		is_ground = true;
	}
	else
	{
		position.y += velocity.y * elapsed_time;
		is_ground = false;
	}
}

void Player::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);
}

void Player::UpdateStateMachine(float elapsed_time)
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

// 移動入力処理
bool Player::InputMove()
{
	// 入力処理
	GamePad& game_pad = GamePad::Instance();
	float axis_x = game_pad.GetAxisLX();
	float axis_y = game_pad.GetAxisLY();

	// カメラの方向
	const Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& camera_front = camera.GetFront();
	const DirectX::XMFLOAT3& camemra_right = camera.GetRight();
	float camera_front_length_xz = sqrtf(camera_front.x * camera_front.x + camera_front.z * camera_front.z);
	float camera_right_length_xz = sqrtf(camemra_right.x * camemra_right.x + camemra_right.z * camemra_right.z);
	float camera_front_x = camera_front.x / camera_front_length_xz;
	float camera_front_z = camera_front.z / camera_front_length_xz;
	float camera_right_x = camemra_right.x / camera_right_length_xz;
	float camera_right_z = camemra_right.z / camera_right_length_xz;

	// 移動ベクトル
	input_move_x = camera_front_x * axis_y + camera_right_x * axis_x;
	input_move_z = camera_front_z * axis_y + camera_right_z * axis_x;
	float vec_length = sqrtf(input_move_x * input_move_x + input_move_z * input_move_z);

	return vec_length > 0.0f;
}

// ジャンプ入力処理
bool Player::InputJump()
{
	GamePad& game_pad = GamePad::Instance();
	if (game_pad.GetButtonDown() & GamePad::BTN_A)
	{
		velocity.y = jump_speed;
		return true;
	}
	return false;
}

void Player::SetState(StateId state_id)
{
	next_state = state_id;
}


void Player::IdleState::OnEnter()
{
	owner->animator->Play("Idle", true);
}

void Player::IdleState::OnUpdate(float elapsed_time)
{
	if (owner->InputMove())
	{
		owner->SetState(StateId::Move);
	}
	if (owner->InputJump())
	{
		owner->SetState(StateId::Jump);
	}
}

void Player::MoveState::OnEnter()
{
	owner->animator->Play("Running", true);
}

void Player::MoveState::OnUpdate(float elapsed_time)
{
	if (!owner->InputMove())
	{
		owner->SetState(StateId::Idle);
	}
	if (owner->InputJump())
	{
		owner->SetState(StateId::Jump);
	}
}

void Player::JumpState::OnEnter()
{
	owner->animator->Play("Jump", false);
}

void Player::JumpState::OnUpdate(float elapsed_time)
{
	bool move = owner->InputMove();

	if (owner->IsGround())
	{
		if (move)
		{
			owner->SetState(StateId::Move);
		}
		else
		{
			owner->SetState(StateId::Idle);
		}
	}
}

