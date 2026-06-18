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
	const DirectX::XMFLOAT3& get_position() const { return position; }

	void SetRotation(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }
	const DirectX::XMFLOAT3& get_rotation() const { return rotation; }

	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	const DirectX::XMFLOAT3& get_scale() const { return scale; }

	void SetColor(const DirectX::XMFLOAT4& color) { this->color = color; }
	const DirectX::XMFLOAT4& get_color() const { return color; }

	bool IsGround() const {return is_ground;}
private:


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

};
