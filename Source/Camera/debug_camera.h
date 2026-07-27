#pragma once

#include <DirectXMath.h>
#include <string>

class DebugCamera
{
public:
	void Update(float elapsed_time);

	void SetEnable(bool enable) { this->enable = enable; }
	bool IsEnable() const { return enable; }

	void DebugGui();

	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	float GetPitch() const { return pitch; }
	float GetYaw() const { return yaw; }

private:
	void Move(float elapsed_time);
	void Rotate();

	void CopyToClipboard(const std::string& text);

	DirectX::XMFLOAT3 GetFocus()const;
private:
	bool enable = false;

	DirectX::XMFLOAT3 position = {0, 5, -10};

	float pitch = 0;
	float yaw = 0;

	float move_speed = 7.0f;
	float rotate_speed = 0.005f;
};