#include "player.h"
#include "Camera/camera.h"
#include "Input/input.h"
#include "Collision/collision_manager.h"

#include "Weapon/projectile_straight.h"
#include "projectile_manager.h"

Player::~Player() = default;

Player::Player(ID3D11Device* device)
{
	const char* filename = ".\\Data\\Model\\Jammo\\Jammo.fbx";
	mesh = std::make_unique<SkinnedMesh>(device, filename, true, 0.0f, axis_system::rhs_y_up);
	animator = std::make_unique<Animator>(mesh.get());
	p_device = device;
	speed = move_speed;

	// ステート生成
	states[static_cast<size_t>(StateId::Idle)] = std::make_unique<IdleState>(this);
	states[static_cast<size_t>(StateId::Move)] = std::make_unique<MoveState>(this);
	states[static_cast<size_t>(StateId::Jump)] = std::make_unique<JumpState>(this);
	states[static_cast<size_t>(StateId::Attack)] = std::make_unique<AttackState>(this);
	states[static_cast<size_t>(StateId::Dash)] = std::make_unique<DashState>(this);
	states[static_cast<size_t>(StateId::WallSlide)] = std::make_unique<WallSlideState>(this);

	SetState(StateId::Idle);
}

void Player::Update(float elapsed_time)
{
	UpdateCharge(elapsed_time);

	UpdateStateMachine(elapsed_time);

	UpdateVelocity(elapsed_time);

	ProjectileManager::Instance().Update(elapsed_time);

	CollisionProjectilesVsEnemys();

	animator->Update(elapsed_time);

	UpdateTransform();
}

void Player::DebugRenderGUI()
{
	ImGui::Begin("ImGUI");
	{
		if (ImGui::CollapsingHeader("player", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("player_position", &position.x);
			ImGui::InputFloat3("player_scale", &scale.x);
			ImGui::Text("move other front: %s", MoveOtherBack() ? "true" : "false");
			ImGui::Separator();
			ImGui::Text("charge shot");
			ImGui::SameLine();
			ImGui::ProgressBar(charge_timer / full_charge_time, ImVec2(-1, 0));
			ImGui::SliderFloat("charge_flash_spead", &flash_speed, 1.0f, 25.0f);
		}
		if (ImGui::CollapsingHeader("any", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("space_division", &space_division);
			CollisionManager::Instance().SetDivision(space_division);
		}
	}
	ImGui::End();
}

void Player::Render(ID3D11DeviceContext* device_context)
{
	mesh->Render(device_context, transform, color, animator->GetCurrentKeyframe());

	ProjectileManager::Instance().Render(device_context);

	ProjectileManager::Instance().DrawDebugPrimitive();
}

void Player::OnLanding()
{
	dash_count = 1;
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

// スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec()
{
	// 入力処理
	GamePad& game_pad = Input::Instance().GetGamePad();
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
	DirectX::XMFLOAT3 vec{};
	vec.x = camera_front_x * axis_y + camera_right_x * axis_x;
	vec.z = camera_front_z * axis_y + camera_right_z * axis_x;

	// スティックが下方向に倒されたとき,ニュートラル以外 true にする(デッドゾーンは後ほど)
	move_otherback = (axis_x != 0.0f || axis_y != 0.0f) && !(axis_y < 0.0f);

	return vec;
}

// 移動入力処理
bool Player::InputMove(float elapsed_time)
{
	if (!enable)
		return false;

	// 進行ベクトル
	DirectX::XMFLOAT3 move_vec = GetMoveVec();

	// 移動
	Move(move_vec.x, move_vec.z, speed);

	// 旋回
	Turn(elapsed_time, move_vec.x, move_vec.z, turn_speed);

	return move_vec.x != 0.0f || move_vec.y != 0.0f || move_vec.z != 0.0f;
}

// ジャンプ入力処理
bool Player::InputJump()
{
	if (!enable)
		return false;

	GamePad& game_pad = Input::Instance().GetGamePad();
	if (game_pad.GetButtonDown() & GamePad::BTN_A)
	{
		velocity.y = jump_speed;
		return true;
	}
	return false;
}

// チャージ判定用
void Player::UpdateCharge(float elapsed_time)
{
	GamePad& game_pad = Input::Instance().GetGamePad();
	if (game_pad.GetButton() & GamePad::BTN_B)
	{
		charge_timer += elapsed_time;

		if (charge_timer > full_charge_time)
		{
			charge_timer = full_charge_time;

			static float color_timer = 0.0f;
			color_timer += elapsed_time;

			float sin_wave = sinf(color_timer * flash_speed * 2.0f * DirectX::XM_PI);

			float blend_ratio = (sin_wave + 1.0f) * 0.5f;

			color.x = 1.0f + blend_ratio * 1.5f;
			color.y = 1.0f + blend_ratio * 1.5f;
			color.z = 1.0f + blend_ratio * 1.5f;
		}
	}
	else
	{
		if (!(game_pad.GetButtonUp() & GamePad::BTN_B))
		{
			charge_timer = 0.0f;
			color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}

// 攻撃入力処理
bool Player::InputAttack()
{
	if (!enable)
		return false;

	GamePad& game_pad = Input::Instance().GetGamePad();

	auto CalculateLaunch = [&](DirectX::XMFLOAT3& out_pos, DirectX::XMFLOAT3& out_dir) {
		out_pos.x = position.x;
		out_pos.y = position.y + ((mesh->GetModelHeight() * scale.y) * 0.15f);
		out_pos.z = position.z;

		if (camera_mode == CameraMode::LockOn) 
		{
			DirectX::XMFLOAT3 enemy_pos = enemy_target->GetPosition();
			enemy_pos.y += 1.2f;

			float dx = enemy_pos.x - out_pos.x;
			float dy = enemy_pos.y - out_pos.y;
			float dz = enemy_pos.z - out_pos.z;
			float dist = sqrtf(dx * dx + dy * dy + dz * dz);

			if (dist > 0.01f) {
				out_dir.x = dx / dist;
				out_dir.y = dy / dist; 
				out_dir.z = dz / dist;
				
				rotation.y = atan2f(dx, dz);
			}
		}
		else {
			out_dir.x = sinf(rotation.y);
			out_dir.y = 0.0f; 
			out_dir.z = cosf(rotation.y);
		}
	};

	if (game_pad.GetButtonDown() & GamePad::BTN_B)
	{
		DirectX::XMFLOAT3 pos{}, dir{};
		CalculateLaunch(pos, dir);

		ProjectileStraight* proj = new ProjectileStraight(&ProjectileManager::Instance(), p_device);
		proj->Launch(dir, pos);

		return true;
	}

	if (game_pad.GetButtonUp() & GamePad::BTN_B)
	{
		if (charge_timer >= full_charge_time)
		{
			DirectX::XMFLOAT3 pos{}, dir{};
			CalculateLaunch(pos, dir);

			ProjectileStraight* proj = new ProjectileStraight(&ProjectileManager::Instance(), p_device);
			proj->ChargeLaunch(dir, pos);

			charge_timer = 0.0f;
			return true;
		}
		charge_timer = 0.0f;
	}

	return false;
}

bool Player::InputDash()
{
	if (!enable)
		return false;

	GamePad& game_pad = Input::Instance().GetGamePad();
	if (game_pad.GetButtonDown() & GamePad::BTN_X)
	{
		if (!IsGround() && dash_count <= 0)
		{
			return false;
		}

		DirectX::XMFLOAT3 dir{};
		dir.x = sinf(rotation.y);
		dir.y = 0.0f; // 上下にはいかないように0固定
		dir.z = cosf(rotation.y);

		if (!IsGround())
		{
			dash_count--;
		}

		speed = dash_speed;
		Move(dir.x, dir.z, speed); // 移動ベクトルを更新しておく

		velocity.x = dir.x * dash_speed;
		velocity.z = dir.z * dash_speed;

		return true;
	}
	return false;
}

void Player::CollisionProjectilesVsEnemys()
{
	int projectile_count = ProjectileManager::Instance().GetProjectileCount();
	for (int i = 0; i < projectile_count; i++)
	{
		Projectile* projectile = ProjectileManager::Instance().GetProjectile(i);

		DirectX::XMFLOAT3 out_position;
		if (CollisionManager::SphereVsCylinder(
			projectile->GetPosition(),
			projectile->GetRadius(),
			enemy_target->GetPosition(),
			enemy_target->GetRadius(),
			enemy_target->GetHeight(),
			out_position))
		{
			if (enemy_target->TakeDamege(1, 0.1f))
			{
				// 弾丸破棄
				projectile->Destroy();
			}
		}
	}
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
	if (owner->InputMove(elapsed_time))
	{
		owner->SetState(StateId::Move);
	}
	if (owner->InputJump())
	{
		owner->SetState(StateId::Jump);
	}
	if (owner->InputAttack())
	{
		owner->SetState(StateId::Attack);
	}
	if (owner->InputDash())
	{
		owner->SetState(StateId::Dash);
	}
}

void Player::MoveState::OnEnter()
{
	owner->animator->Play("Running", true);
}

void Player::MoveState::OnUpdate(float elapsed_time)
{
	if (!owner->InputMove(elapsed_time))
	{
		owner->SetState(StateId::Idle);
	}
	if (owner->InputJump())
	{
		owner->SetState(StateId::Jump);
	}
	if (owner->InputAttack())
	{
		owner->SetState(StateId::Attack);
	}
	if (owner->InputDash())
	{
		owner->SetState(StateId::Dash);
	}

	if (!owner->IsGround() && owner->IsWallTouch())
	{
		// 入力方向と壁の法線の内積をチェック
		DirectX::XMFLOAT3 move_vec = owner->GetMoveVec();
		float dot = move_vec.x * owner->GetWallNormal().x + move_vec.z * owner->GetWallNormal().z;
		if (dot < -0.1f) // 壁に向かって入力されている
		{
			owner->SetState(StateId::WallSlide);
		}
	}
}

void Player::JumpState::OnEnter()
{
	owner->animator->Play("Jump", false);
}

void Player::JumpState::OnUpdate(float elapsed_time)
{
	bool move = owner->InputMove(elapsed_time);

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
	if (owner->InputDash())
	{
		owner->SetState(StateId::Dash);
	}

	if (!owner->IsGround() && owner->IsWallTouch())
	{
		// 入力方向と壁の法線の内積をチェック
		DirectX::XMFLOAT3 move_vec = owner->GetMoveVec();
		float dot = move_vec.x * owner->GetWallNormal().x + move_vec.z * owner->GetWallNormal().z;
		if (dot < -0.1f) // 壁に向かって入力されている
		{
			owner->SetState(StateId::WallSlide);
		}
	}
}

void Player::AttackState::OnEnter()
{
	owner->animator->Play("FastShoot", false);
}

void Player::AttackState::OnUpdate(float elapsed_time)
{
	bool move = owner->InputMove(elapsed_time);

	if (owner->InputJump())
	{
		owner->SetState(StateId::Jump);
	}
	if (owner->animator->IsFinished())
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

void Player::DashState::OnEnter()
{
	owner->animator->Play("Frying", false);
}

void Player::DashState::OnUpdate(float elapsed_time)
{
	owner->velocity.y = 0.0f;

	if (owner->InputJump())
	{
		owner->speed = owner->move_speed;
		owner->SetState(StateId::Jump);
	}

	if (owner->animator->IsFinished())
	{
		owner->speed = owner->move_speed;
		if (owner->InputMove(elapsed_time))
		{
			owner->SetState(StateId::Move);
		}
		else
		{
			owner->SetState(StateId::Idle);
		}
	}
}

void Player::WallSlideState::OnEnter()
{
	owner->animator->Play("HangIdle", true);
	owner->dash_count = 1;
}

void Player::WallSlideState::OnUpdate(float elapsed_time)
{
	// 落下速度を遅くする
	if (owner->velocity.y < -2.0f)
	{
		owner->velocity.y = -2.0f;
	}

	// 地面に着いたらIdleかMoveへ
	if (owner->IsGround())
	{
		owner->SetState(StateId::Idle);
		return;
	}

	// 壁蹴り（ジャンプ）入力
	GamePad& game_pad = Input::Instance().GetGamePad();
	if (game_pad.GetButtonDown() & GamePad::BTN_A)
	{
		// 壁の法線方向へ弾き飛ばしつつ、上方向へジャンプ
		owner->velocity.y = owner->jump_speed * 1.2f; // 少し高めに
		owner->velocity.x = owner->GetWallNormal().x * owner->move_speed;
		owner->velocity.z = owner->GetWallNormal().z * owner->move_speed;

		owner->SetState(StateId::Jump);
		return;
	}

	// 壁から離れた、または入力がやんだらJump状態（空中）へ戻る
	DirectX::XMFLOAT3 move_vec = owner->GetMoveVec();
	float dot = move_vec.x * owner->GetWallNormal().x + move_vec.z * owner->GetWallNormal().z;
	if (!owner->IsWallTouch() || dot >= -0.1f)
	{
		owner->SetState(StateId::Jump);
	}
}