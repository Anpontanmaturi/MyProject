#pragma once

#include <Windows.h>

using MouseButtons = unsigned int;

// マウス
class Mouse
{
public:
	static const MouseButtons BTN_LEFT = (1 << 0);
	static const MouseButtons BTN_RIGHT = (1 << 1);
	static const MouseButtons BTN_MIDDLE = (1 << 2);

public:
	Mouse(HWND hwnd);
	~Mouse() {}

	void Update();

	// ボタンの状態を取得
	MouseButtons GetButton() const { return button_state[0]; } // 入力
	MouseButtons GetButtonDown() const { return button_down; } // 押下
	MouseButtons GetButtonUp() const { return button_up; }		// 押上

	// マウスホイール値の設定・取得
	void SetWheel(int wheel) { this->wheel[0] += wheel; }
	int GetWheel()const { return wheel[1]; }

	// マウスカーソル座標取得
	int GetPositionX() const { return position_x[0]; }
	int GetPositionY() const { return position_y[0]; }
	// 前回の座標
	int GetOldPositionX() const { return position_x[1]; }
	int GetOldPositionY() const { return position_y[1]; }

	// スクリーン系の設定・取得
	void SetScreenWidth(int width) { screen_width = width; }
	void SetScreenHeight(int height) { screen_height = height; }
	int GetScreenWidth() const { return screen_width; }
	int GetScreenHeight() const { return screen_height; }

private:
	MouseButtons	button_state[2] = {};
	MouseButtons	button_down = {};
	MouseButtons	button_up = {};
	int				position_x[2] = {};
	int				position_y[2] = {};
	int				wheel[2] = {};
	int				screen_width = 0;
	int				screen_height = 0;
	HWND			hWnd = nullptr;
};
