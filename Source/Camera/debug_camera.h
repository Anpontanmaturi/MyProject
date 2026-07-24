#pragma once

#include <DirectXMath.h>

class DebugCamera
{
public:
	void Update(float elapsed_time);

	void SetEnable(bool enable);
	bool IsEnable() const;

	void DebugGui();

private:
	void Move();
	void Rotate();

private:
	bool enable = false;

	DirectX::XMFLOAT3 position = {0, 5, -10};

	float pitch = 0;
	float yaw = 0;

	float move_speed = 10.0f;
	float rotate_speed = 2.0f;
};