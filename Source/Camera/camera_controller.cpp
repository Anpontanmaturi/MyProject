#include "Input/game_pad.h"
#include "camera_controller.h"
#include "camera.h"

// 更新処理
void CameraController::update(float elapsed_time)
{
	GamePad& game_pad = GamePad::Instance();
	float ax = game_pad.get_axis_rx();
	float ay = game_pad.get_axis_ry();
	// カメラの回転速度
	float speed = roll_speed * elapsed_time;

	// スティックの入力値に合わせてX軸とY軸を回転
	angle.x += ay * speed;
	angle.y += ax * speed;

	// X軸のカメラ回転を制限
	if (angle.x < min_angle_x)
	{
		angle.x = min_angle_x;
	}
	if (angle.x > max_angle_x)
	{
		angle.x = max_angle_x;
	}

	// Y軸の回転値を-3.14～3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI)
	{
		angle.y += DirectX::XM_2PI;
	}
	if (angle.y > DirectX::XM_PI)
	{
		angle.y -= DirectX::XM_2PI;
	}

	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	// 補間処理
	float t = interpolation_speed * elapsed_time;
	Camera& camera = Camera::Instance();
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&camera.get_eye());
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&camera.get_focus());
	DirectX::XMVECTOR FinalEye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR FinalFocus = DirectX::XMLoadFloat3(&target);
	DirectX::XMStoreFloat3(&eye, DirectX::XMVectorLerp(Eye, FinalEye, t));
	DirectX::XMStoreFloat3(&target, DirectX::XMVectorLerp(Focus, FinalFocus, t));

	// カメラの視点と注視点を設定
	camera.set_lookat(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}
