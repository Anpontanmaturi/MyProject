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

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive();

	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	float GetRadius()const { return radius; }

	void SetScale(float scale) { this->scale = { scale, scale, scale }; }

	void SetOwner(Character* owner) { this->owner = owner; }
	Character* GetOwner() const { return owner; }

	void SetSpeed(float speed) { this->speed = speed; }
	float GetSpeed() const { return speed; }

	// 破棄
	void Destroy();

protected:
	void UpdateTransform();

protected:
	DirectX::XMFLOAT3		position = {};
	DirectX::XMFLOAT3		direction = { 0, 0, 1 };
	DirectX::XMFLOAT3		scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4		transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	DirectX::XMFLOAT4		color = { 1, 1, 1, 1 };
	float					speed = 10.0f;
	float					life_timer = 3.0f;
	ProjectileManager*		manager = nullptr;
	float					radius = 0.2f;
	Character*				owner = nullptr;
};