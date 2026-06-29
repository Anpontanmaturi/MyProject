#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
	// 更新処理
	void Update(float elapsed_time);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3 player_rot_y) {
		this->target = target; this->player_rotation = player_rot_y;
	}

	void SetMove(bool player_move) { this->player_move = player_move; }

private:
	DirectX::XMFLOAT3		target = { 0, 0, 0 };	// 注視点
	DirectX::XMFLOAT3		angle = { DirectX::XMConvertToRadians(15), 0, 0 };	// 回転角度
	float					roll_speed = DirectX::XMConvertToRadians(90); // 回転速度
	float					range = 5.0f; 	// 距離
	float					max_angle_x = DirectX::XMConvertToRadians(75);
	float					min_angle_x = DirectX::XMConvertToRadians(10);
	float					interpolation_speed = 3.0f; // 補間速度

	DirectX::XMFLOAT3		player_rotation;
	bool					player_move;
};
