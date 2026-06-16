#pragma once
#include "camera.h"

class FreeCameraController
{
public:
	// カメラからコントローラーへパラメータを同期する
	void sync_camera_to_controller(const Camera& camera);

	// コントローラーからカメラへパラメータを同期する
	void sync_controller_to_camera(Camera& camera);

	// 更新処理
	void update();

private:
	DirectX::XMFLOAT3		eye;
	DirectX::XMFLOAT3		focus;
	DirectX::XMFLOAT3		up;
	DirectX::XMFLOAT3		right;
	float					distance;

	float					angleX;
	float					angleY;
};
