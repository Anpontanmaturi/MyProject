#include "character.h"
#include <Collision/collision_manager.h>

void Character::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);
}

void Character::UpdateVelocity(float elapsed_time)
{
	// 加速処理
	float input_move_length = sqrtf(input_move_x * input_move_x + input_move_z * input_move_z);
	if (input_move_length > 0)
	{
		float vec_x = input_move_x / input_move_length;
		float vec_z = input_move_z / input_move_length;

		float acceleration = this->acceleration * elapsed_time;
		if (!is_ground) acceleration *= air_control;

		velocity.x += vec_x * acceleration;
		velocity.z += vec_z * acceleration;

		// 最大速度制限
		float velocity_length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
		if (velocity_length > max_move_speed)
		{
			velocity.x = (velocity.x / velocity_length) * max_move_speed;
			velocity.z = (velocity.z / velocity_length) * max_move_speed;
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
			is_wall_touch = true;
			wall_normal = hit_result.normal;

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

			// 壁に当たらなかったらフラグを寝かせる
			is_wall_touch = false;
		}
	}

	// 上下移動処理
	DirectX::XMFLOAT3 start = { position.x, position.y + 1, position.z };
	DirectX::XMFLOAT3 end = { position.x, position.y + move_y, position.z };
	HitResult hit_result;
	if (CollisionManager::Instance().Raycast(start, end, hit_result))
	{
		position.y = hit_result.position.y;

		if (!is_ground)
		{
			OnLanding();
		}
		velocity.y = 0.0f;
		is_ground = true;
	}
	else
	{
		position.y += velocity.y * elapsed_time;
		is_ground = false;
	}
}

bool Character::TakeDamege(int damage, float invincible_time)
{
	// ダメージが0の場合は健康状態を変更する必要がない
	if (damage == 0) return false;

	// 無敵時間中はダメージを与えない
	if (this->invincible_timer > 0.0f)return false;

	// 無敵時間設定
	this->invincible_timer = invincible_time;

	OnDamaged();

	return true;
}

// 無敵時間更新
void Character::UpdateInvincibleTimer(float elapsed_time)
{
	if (invincible_timer > 0.0f)
	{
		invincible_timer -= elapsed_time;
	}
}

// 移動
void Character::Move(float vx, float vz, float speed)
{
	input_move_x = vx;
	input_move_z = vz;

	max_move_speed = speed;
}

// 旋回
void Character::Turn(float elapsed_time, float vx, float vz, float speed)
{
	speed *= elapsed_time;

	// 進行ベクトルがゼロベクトルの場合は処理しない
	float length = sqrtf(vx * vx + vz * vz);
	if (length <= 0.001f)
	{
		return;
	}
	vx /= length;
	vz /= length;

	// 進行方向に向ける
	{
		// 向いている方向
		float front_x = sinf(rotation.y);
		float front_z = cosf(rotation.y);

		// 回転量調整
		float dot = front_x * vx + front_z * vz;
		float rot = 1.0f - dot;

		// 左右判定をして回転処理
		float cross = front_x * vz - front_z * vx;
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

// ジャンプ処理
void Character::Jump(float speed)
{
	// 上方向の力を設定
	velocity.y = speed;
}