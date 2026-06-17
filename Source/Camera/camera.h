#pragma once

#include <DirectXMath.h>

class Camera
{
private:
	Camera();
	~Camera() {}
public:
	// インスタンス取得
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}

	// 指定方向を向く
	void set_lookat(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	// パースペクティブ設定
	void set_perspectice_fov(float fovY, float aspect, float nearZ, float farZ);

	// 平行投影
	void set_orthographic(float width, float height, float nearZ, float farZ);

	// ビュー行列取得
	const DirectX::XMFLOAT4X4& get_view() const { return view; }

	//プロジェクション行列
	const DirectX::XMFLOAT4X4& get_projection() const { return projection; }

	// 視点取得
	const DirectX::XMFLOAT3& get_eye()const { return eye; }

	// 注視点取得
	const DirectX::XMFLOAT3& get_focus()const { return focus; }

	// 上方向取得
	const DirectX::XMFLOAT3& get_up()const { return up; }

	// 前方向取得
	const DirectX::XMFLOAT3& get_front()const { return front; }

	// 右方向取得
	const DirectX::XMFLOAT3& get_right()const { return right; }

private:
	DirectX::XMFLOAT4X4	view;
	DirectX::XMFLOAT4X4	projection;

	DirectX::XMFLOAT3	eye;
	DirectX::XMFLOAT3	focus;
	DirectX::XMFLOAT3	up;
	DirectX::XMFLOAT3	front;
	DirectX::XMFLOAT3	right;
};