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
	void UpdateTransform();

	// •`‰æ
	void Render(ID3D11DeviceContext* device_context);

	// Ý’èEŽæ“¾
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	void SetRotation(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }
	const DirectX::XMFLOAT3& GetRotation() const { return rotation; }
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	void SetColor(const DirectX::XMFLOAT4& color) { this->color = color; }
	const DirectX::XMFLOAT4& GetColor() const { return color; }

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