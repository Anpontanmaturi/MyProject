#pragma once
#include "Graphics/graphics.h"

class ProjectileManager;

class Projectile
{
public:
	Projectile(ProjectileManager* manager);
	virtual ~Projectile(){}

	virtual void Update(float elapsed_time) = 0;

	virtual void Render(ID3D11DeviceContext* device_context) = 0;

	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	float GetRadius()const { return radius; }

	// ”jŠü
	void Destroy();

protected:
	void UpdateTransform();

protected:
	DirectX::XMFLOAT3		position = {};
	DirectX::XMFLOAT3		direction = { 0, 0, 1 };
	DirectX::XMFLOAT3		scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4		transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	DirectX::XMFLOAT4		color = { 1, 1, 1, 1 };
	ProjectileManager*		manager = nullptr;
	float					radius = 1.0f;

};