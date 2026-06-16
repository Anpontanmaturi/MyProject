#pragma once

#include <DirectXMath.h>

class GameCameraController
{
public:
	GameCameraController() {}
	~GameCameraController() {}

	// 更新
	void update(float elapsed_time);

	// ターゲットの位置を設定
	void set_target(const DirectX::XMFLOAT3& target) { this->target = target; }

private:
	DirectX::XMFLOAT3 target = {};
	DirectX::XMFLOAT3 angle = {};
	float	roll_speed = DirectX::XMConvertToRadians(90);
	float	range = 10.0f;

	float	maxAngleX = DirectX::XMConvertToRadians(45);
	float	minAngleX = DirectX::XMConvertToRadians(-45);
};