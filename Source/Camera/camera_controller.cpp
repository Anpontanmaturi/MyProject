#include "Input/game_pad.h"
#include "camera_controller.h"
#include "camera.h"
#include "Character/sandbag.h"

// 更新処理
void CameraController::Update(float elapsed_time)
{
	GamePad& game_pad = GamePad::Instance();
	float ax = game_pad.GetAxisRX();
	float ay = game_pad.GetAxisRY();
	// カメラの回転速度
	float speed = roll_speed * elapsed_time;

	DirectX::XMFLOAT3 eye{};
	DirectX::XMFLOAT3 focus = target;

	switch (camera_mode)
	{
	case CameraMode::Normal:
		if (ax || ay)
		{
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
		}
		else if (player_move)
		{
			float diff = player_rotation.y - angle.y;
			while (diff < -DirectX::XM_PI) diff += DirectX::XM_2PI;
			while (diff > DirectX::XM_PI) diff -= DirectX::XM_2PI;

			angle.y += diff * (interpolation_speed * 0.5f) * elapsed_time;
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
		eye.x = target.x - front.x * range;
		eye.y = target.y - front.y * range;
		eye.z = target.z - front.z * range;

		focus = target;
		break;

	case CameraMode::LockOn:
		DirectX::XMVECTOR PlayerPos = DirectX::XMLoadFloat3(&target);
		DirectX::XMVECTOR EnemyPos = DirectX::XMLoadFloat3(lockon_target);

		// プレイヤーから敵への方向ベクトルを計算
		DirectX::XMVECTOR LookDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(EnemyPos, PlayerPos));

		// カメラ位置：プレイヤーから「敵と反対の方向」にrange分だけ離す（プレイヤーの後ろ）
		DirectX::XMVECTOR EyePos = DirectX::XMVectorSubtract(PlayerPos, DirectX::XMVectorScale(LookDir, range));
		DirectX::XMStoreFloat3(&eye, EyePos);

		// 注視点をサンドバッグ（敵）にする
		focus = *lockon_target;
		focus.y += 1.0f;

		break;
	}

	// 補間処理
	float t = interpolation_speed * elapsed_time;
	Camera& camera = Camera::Instance();
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&camera.GetEye());
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&camera.GetFocus());
	DirectX::XMVECTOR FinalEye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR FinalFocus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMStoreFloat3(&eye, DirectX::XMVectorLerp(Eye, FinalEye, t));
	DirectX::XMStoreFloat3(&target, DirectX::XMVectorLerp(Focus, FinalFocus, t));

	// カメラの視点と注視点を設定
	camera.SetLookat(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}
