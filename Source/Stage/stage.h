#pragma once

#include "Graphics/skinned_mesh.h"
#include "Graphics/gltf_model.h"
#include "Collision/collision_manager.h"

class Stage
{
public:
	Stage(ID3D11Device* device);
	~Stage();

	// XV
	void update_transform();

	// •`‰æ
	void render(ID3D11DeviceContext* device_context);

	// Ý’èEŽæ“¾
	void set_position(const DirectX::XMFLOAT3& position) { this->position = position; }
	const DirectX::XMFLOAT3& get_position() const { return position; }
	void set_rotation(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }
	const DirectX::XMFLOAT3& get_rotation() const { return rotation; }
	void set_scale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	const DirectX::XMFLOAT3& get_scale() const { return scale; }
	void set_color(const DirectX::XMFLOAT4& color) { this->color = color; }
	const DirectX::XMFLOAT4& get_color() const { return color; }

private:
	std::unique_ptr<SkinnedMesh> model;
	CollisionMesh	collision_mesh;
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT3 rotation = {};
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 transform =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
};