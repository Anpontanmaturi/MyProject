#include "sandbag.h"
#include "Collision/collision_manager.h"
#include "Graphics/graphics.h"

Sandbag::~Sandbag() = default;

Sandbag::Sandbag(ID3D11Device* device)
{
	const char* filename = ".\\Data\\Model\\Jammo\\Jammo.fbx";
	mesh = std::make_unique<SkinnedMesh>(device, filename, true, 0.0f, axis_system::rhs_y_up);
	animator = std::make_unique<Animator>(mesh.get());

	// ステート生成
	states[static_cast<size_t>(StateId::Idle)] = std::make_unique<IdleState>(this);

	SetState(StateId::Idle);
}

void Sandbag::Update(float elapsed_time)
{
	UpdateStateMachine(elapsed_time);

	UpdateVelocity(elapsed_time);

	UpdateInvincibleTimer(elapsed_time);

	animator->Update(elapsed_time);

	UpdateTransform();
}

void Sandbag::Render(ID3D11DeviceContext* device_context)
{
	mesh->Render(device_context, transform, color, animator->GetCurrentKeyframe());

	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(1, 0, 0, 1));
}

void Sandbag::UpdateVelocity(float elapsed_time)
{
	
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

void Sandbag::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMStoreFloat4x4(&transform, S * R * T);
}

bool Sandbag::TakeDamege(int damage, float invincible_time)
{
	// ダメージが0の場合は健康状態を変更する必要がない
	if (damage == 0) return false;

	// 無敵時間中はダメージを与えない
	if (invincible_timer > 0.0f)return false;

	// 無敵時間設定
	invincible_timer = invincible_time;

	animator->Play("GetHit1", false); // 後ほどステートにする

	return true;
}

// 無敵時間更新
void Sandbag::UpdateInvincibleTimer(float elapsed_time)
{
	if (invincible_timer > 0.0f)
	{
		invincible_timer -= elapsed_time;
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