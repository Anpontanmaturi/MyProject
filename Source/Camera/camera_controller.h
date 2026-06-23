#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
	// 更新処理
	void Update(float elapsed_time);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

private:
	DirectX::XMFLOAT3		target = { 0, 0, 0 };	// 注視点
	DirectX::XMFLOAT3		angle = { DirectX::XMConvertToRadians(15), 0, 0 };	// 回転角度
	float					roll_speed = DirectX::XMConvertToRadians(90); // 回転速度
	float					range = 10.0f; 	// 距離
	float					max_angle_x = DirectX::XMConvertToRadians(45);
	float					min_angle_x = DirectX::XMConvertToRadians(10);
	float					interpolation_speed = 3.0f; // 補間速度
};
