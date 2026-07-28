#include "debug_camera.h"
#include <DirectXMath.h>
#include <Windows.h>
#include <string>
#include <cassert>

#include "Input/input.h"
#include "../../imgui/imgui.h"
#include "Camera/camera.h"

void DebugCamera::Update(float elapsed_time)
{
	if (!enable)
	{
		return;
	}

	Rotate();
	Move(elapsed_time);

	Camera::Instance().SetLookat(
		position,
		GetFocus(),
		{ 0,1,0 });
}

// 回転
void DebugCamera::Rotate()
{
	Mouse& mouse = Input::Instance().GetMouse();
	if (!(mouse.GetButton() & Mouse::BTN_RIGHT))
	{
		return;
	}

	int dx = mouse.GetPositionX() - mouse.GetOldPositionX();
	int dy = mouse.GetPositionY() - mouse.GetOldPositionY();

	yaw += dx * rotate_speed;
	pitch += dy * rotate_speed;

	const float limit = DirectX::XM_PIDIV2 - 0.01f;

	if (pitch > limit) pitch = limit;
	if (pitch < -limit) pitch = -limit;
}

// 移動
void DebugCamera::Move(float elapsed_time)
{
	Mouse& mouse = Input::Instance().GetMouse();
	float speed = move_speed;

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		speed *= 3.0f;
	}
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		speed *= 0.2f;
	}

	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(yaw);

	DirectX::XMVECTOR forward = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(0, 0, 1, 0), rot);
	DirectX::XMVECTOR right = DirectX::XMVector3TransformNormal(DirectX::XMVectorSet(1, 0, 0, 0), rot);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);

	if (GetAsyncKeyState('W') & 0x8000)// 前
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(forward, speed * elapsed_time));
	if (GetAsyncKeyState('S') & 0x8000)// 後
		pos = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(forward, speed * elapsed_time));
	if (GetAsyncKeyState('A') & 0x8000)// 左
		pos = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(right, speed * elapsed_time));
	if (GetAsyncKeyState('D') & 0x8000)// 右
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(right, speed * elapsed_time));
	if (GetAsyncKeyState('Q') & 0x8000)// 上昇
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(up, speed * elapsed_time));
	if (GetAsyncKeyState('E') & 0x8000)// 下降
		pos = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(up, speed * elapsed_time));

	DirectX::XMStoreFloat3(&position, pos);

	int wheel = mouse.GetWheel();

	if (wheel != 0)
	{
		move_speed += wheel * 0.01f;

		if (move_speed < 1.0f)
			move_speed = 1.0f;
	}
}

DirectX::XMFLOAT3 DebugCamera::GetFocus() const
{
	DirectX::XMMATRIX rot =
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0);

	DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&position);

	DirectX::XMVECTOR forward =
		DirectX::XMVector3TransformNormal(
			DirectX::XMVectorSet(0, 0, 1, 0),
			rot);

	DirectX::XMVECTOR target = DirectX::XMVectorAdd(eye, forward);

	DirectX::XMFLOAT3 focus;
	DirectX::XMStoreFloat3(&focus, target);

	return focus;
}

// クリップボードにコピー
void DebugCamera::CopyToClipboard(const std::string& text)
{
	if (!OpenClipboard(nullptr))
		return;

	EmptyClipboard();

	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);

	if (hMem)
	{
		char* ptr = static_cast<char*>(GlobalLock(hMem));
		
		if (ptr)
		{
			memcpy(ptr, text.c_str(), text.size() + 1);

			GlobalUnlock(hMem);

			SetClipboardData(CF_TEXT, hMem);
		}
	}

	CloseClipboard();
}

void DebugCamera::DebugGui()
{
	ImGui::Begin("Camera");
	if (ImGui::CollapsingHeader("free_camera", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
	{
		if(ImGui::Checkbox("Enable", &enable))
		{
			Camera& camera = Camera::Instance();

			position = camera.GetEye();

			const auto& front = camera.GetFront();

			yaw = atan2f(front.x, front.z);

			float length =
				sqrtf(front.x * front.x + front.z * front.z);

			pitch = -(atan2f(front.y, length));
		}

		if (ImGui::CollapsingHeader("copy_coordinate", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto focus = GetFocus();
			if (ImGui::Button("Camera"))
			{
				char buffer[256];

				sprintf_s(
					buffer,
					"Camera::Instance().SetLookat(\n"
					"    { %.3ff, %.3ff, %.3ff },\n"
					"    { %.3ff, %.3ff, %.3ff },\n"
					"    { 0, 1, 0 });",
					position.x,
					position.y,
					position.z,
					focus.x,
					focus.y,
					focus.z);

				CopyToClipboard(buffer);
			}
			ImGui::SameLine();

			if (ImGui::Button("Position"))
			{
				char buffer[256];

				sprintf_s(
					buffer,
					"%.3ff, %.3ff, %.3ff\n",
					position.x,
					position.y,
					position.z);

				CopyToClipboard(buffer);
			}
			ImGui::SameLine();

			if (ImGui::Button("Look"))
			{
				char buffer[256];

				sprintf_s(
					buffer,
					"%.3ff, %.3ff, %.3ff\n",
					focus.x,
					focus.y,
					focus.z);

				CopyToClipboard(buffer);
			}
		}

		ImGui::DragFloat3("Position", &position.x, 0.1f);

		ImGui::DragFloat("Pitch", &pitch, 0.01f);

		ImGui::DragFloat("Yaw", &yaw, 0.01f);
	}
	ImGui::End();
}