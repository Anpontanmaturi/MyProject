#pragma once

#include<DirectXMath.h>

class Character
{
public:
	Character() {}
	virtual ~Character() {}

	void UpdateTransform();

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

	virtual bool TakeDamege(int damage, float invincible_time);

protected:
	// 移動
	void Move(float vx, float vy, float speed);

	// 旋回
	void Turn(float elapsed_time, float vx, float vz, float speed);

	// ジャンプ
	void Jump(float speed);

	// ダメージを受けた時に呼ばれる
	virtual void OnDamaged() {}

	// 速力更新
	void UpdateVelocity(float elapsed_time);

	// 無敵時間更新
	void UpdateInvincibleTimer(float elapsedTime);

	// 着地した時に呼ばれる
	virtual void OnLanding() {}

protected:
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
	float	radius = 0.5f;
	float	height = 1.5f;
	float	acceleration = 50.0f;
	float	deceleration = 20.0f;
	float	input_move_x = 0.0f;
	float	input_move_z = 0.0f;
	float	max_move_speed = 20.0f;
	bool	is_ground = false;
	float	invincible_timer = 1.0f;
	float	jump_speed = 5.0f;
	float	air_control = 0.3f;
};