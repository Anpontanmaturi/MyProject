#include "projectile.h"
#include "projectile_manager.h"

Projectile::Projectile(ProjectileManager* manager) : manager(manager)
{
	manager->Register(this);
}

// 行列更新
void Projectile::UpdateTransform()
{
	DirectX::XMVECTOR Front, Up, Right;

	// 前ベクトル算出
	Front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction));

	// 仮上ベクトル算出
	Up = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0.001f, 1.0f, 0.0f, 0.0f));

	// 右ベクトル算出
	Right = DirectX::XMVector3Cross(Up, Front);

	// 上ベクトル算出
	Up = DirectX::XMVector3Cross(Front, Right);

	// 計算結果の取り出し
	DirectX::XMFLOAT3 right, up, front;
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	// 行列の作成
	transform._11 = right.x * scale.x;
	transform._12 = right.y * scale.x;
	transform._13 = right.z * scale.x;
	transform._14 = 0.0f;
	transform._21 = up.x * scale.y;
	transform._22 = up.y * scale.y;
	transform._23 = up.z * scale.y;
	transform._24 = 0.0f;
	transform._31 = front.x * scale.z;
	transform._32 = front.y * scale.z;
	transform._33 = front.z * scale.z;
	transform._34 = 0.0f;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

	// 発射方向
	this->direction = front;
}

void Projectile::Destroy()
{
	manager->Remove(this);
}