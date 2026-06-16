#include <windows.h>
#include <math.h>
#include <Xinput.h>
#include "game_pad.h"

// 更新
void GamePad::update()
{
	axis_lx = axis_ly = 0.0f;
	axis_rx = axis_ry = 0.0f;
	trigger_l = trigger_r = 0.0f;

	GamePadButtons new_button_state = 0;

	XINPUT_STATE state{};
	if (XInputGetState(slot, &state) == ERROR_SUCCESS)
	{
		XINPUT_GAMEPAD& gamepad = state.Gamepad;

		if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)					new_button_state |= BTN_UP;
		if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)				new_button_state |= BTN_RIGHT;
		if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)				new_button_state |= BTN_DOWN;
		if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)				new_button_state |= BTN_LEFT;
		if (gamepad.wButtons & XINPUT_GAMEPAD_A)						new_button_state |= BTN_A;
		if (gamepad.wButtons & XINPUT_GAMEPAD_B)						new_button_state |= BTN_B;
		if (gamepad.wButtons & XINPUT_GAMEPAD_X)						new_button_state |= BTN_X;
		if (gamepad.wButtons & XINPUT_GAMEPAD_Y)						new_button_state |= BTN_Y;
		if (gamepad.wButtons & XINPUT_GAMEPAD_START)					new_button_state |= BTN_START;
		if (gamepad.wButtons & XINPUT_GAMEPAD_BACK)						new_button_state |= BTN_BACK;
		if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)				new_button_state |= BTN_LEFT_THUMB;
		if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)				new_button_state |= BTN_RIGHT_THUMB;
		if (gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)			new_button_state |= BTN_LEFT_SHOULDER;
		if (gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)			new_button_state |= BTN_RIGHT_SHOULDER;
		if (gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)	new_button_state |= BTN_LEFT_TRIGGER;
		if (gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)	new_button_state |= BTN_RIGHT_TRIGGER;

		if ((gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
			(gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			gamepad.sThumbLX = 0;
			gamepad.sThumbLY = 0;
		}

		if ((gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
			(gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			gamepad.sThumbRX = 0;
			gamepad.sThumbRY = 0;
		}

		trigger_l = static_cast<float>(gamepad.bLeftTrigger) / 255.0f;
		trigger_r = static_cast<float>(gamepad.bRightTrigger) / 255.0f;
		axis_lx = static_cast<float>(gamepad.sThumbLX) / static_cast<float>(0x8000);
		axis_ly = static_cast<float>(gamepad.sThumbLY) / static_cast<float>(0x8000);
		axis_rx = static_cast<float>(gamepad.sThumbRX) / static_cast<float>(0x8000);
		axis_ry = static_cast<float>(gamepad.sThumbRY) / static_cast<float>(0x8000);

	}
	else
	{
		// 今は一旦無し
	}

	// キーボード対応
	{
		float lx = 0.0f;
		float ly = 0.0f;
		float rx = 0.0f;
		float ry = 0.0f;
		if (GetAsyncKeyState('W') & 0x8000) ly = 1.0f;
		if (GetAsyncKeyState('A') & 0x8000) lx = -1.0f;
		if (GetAsyncKeyState('S') & 0x8000) ly = -1.0f;
		if (GetAsyncKeyState('D') & 0x8000) lx = 1.0f;
		if (GetAsyncKeyState('I') & 0x8000) ry = 1.0f;
		if (GetAsyncKeyState('J') & 0x8000) rx = -1.0f;
		if (GetAsyncKeyState('K') & 0x8000) ry = -1.0f;
		if (GetAsyncKeyState('L') & 0x8000) rx = 1.0f;
		if (GetAsyncKeyState('Z') & 0x8000) new_button_state |= BTN_A;
		if (GetAsyncKeyState('X') & 0x8000) new_button_state |= BTN_B;
		if (GetAsyncKeyState('C') & 0x8000) new_button_state |= BTN_X;
		if (GetAsyncKeyState('V') & 0x8000) new_button_state |= BTN_Y;
		if (GetAsyncKeyState(VK_UP) & 0x8000)	new_button_state |= BTN_UP;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)	new_button_state |= BTN_RIGHT;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)	new_button_state |= BTN_DOWN;
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)	new_button_state |= BTN_LEFT;

		if (new_button_state & BTN_UP)    ly = 1.0f;
		if (new_button_state & BTN_RIGHT) lx = 1.0f;
		if (new_button_state & BTN_DOWN)  ly = -1.0f;
		if (new_button_state & BTN_LEFT)  lx = -1.0f;

		if (lx >= 1.0f || lx <= -1.0f || ly >= 1.0f || ly <= -1.0)
		{
			float power = ::sqrtf(lx * lx + ly * ly);
			axis_lx = lx / power;
			axis_ly = ly / power;
		}

		if (rx >= 1.0f || rx <= -1.0f || ry >= 1.0f || ry <= -1.0)
		{
			float power = ::sqrtf(rx * rx + ry * ry);
			axis_rx = rx / power;
			axis_ry = ry / power;
		}
	}

	// ボタン情報の更新
	{
		button_state[1] = button_state[0];	// スイッチ履歴
		button_state[0] = new_button_state;

		button_down = ~button_state[1] & new_button_state;	// 押した瞬間
		button_up = ~new_button_state & button_state[1];	// 離した瞬間
	}
}