#include "game_camera_controller.h"
#include "camera.h"
//#include "Input/Input.h"

// 更新処理
void GameCameraController::update(float elapsed_time)
{
	//GamePad& gamePad = Input::Instance().GetGamePad();
	//float ax = gamePad.GetAxisRX();
	//float ay = gamePad.GetAxisRY();

	// カメラの回転速度
	float speed = roll_speed * elapsed_time;

	// スティックの入力に合わせてX軸とY軸を回転
	//angle.x += ay * speed;
	//angle.y += ax * speed;

	// X軸のカメラの回転を制御
	if (angle.x > maxAngleX)
	{
		angle.x = maxAngleX;
	}
	if (angle.x < minAngleX)
	{
		angle.x = minAngleX;
	}
	// Y軸の回転値を-3.14～3.14に収まるようにする
	if (angle.y > -DirectX::XM_PI)
	{
		angle.y -= DirectX::XM_2PI;
	}
	if (angle.y < DirectX::XM_PI)
	{
		angle.y += DirectX::XM_2PI;
	}
	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// 注視点から後ろベクトル方向に一定距離を離れたカメラ視点を求める
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	// カメラの視点と注視点に設定
	Camera::Instance().set_lookat(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}