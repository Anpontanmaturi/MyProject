#pragma once

#include <Windows.h>

using MouseButton = unsigned int;

class Mouse
{
public:
	static const MouseButton BTN_LEFT = (1 << 0);
	static const MouseButton BTN_MIDDLE = (1 << 1);
	static const MouseButton BTN_RIGHT = (1 << 2);

public:
	Mouse(HWND hWnd);
	~Mouse(){}

	void Update();

	// ボタン入力状態の取得
	MouseButton GetButton() const { return button_state[0]; }

	// ボタン押下状態の取得
	MouseButton GetButtonDown() const { return button_down; }

	// ボタン押上状態の取得
	MouseButton GetButtonUp() const { return button_up; }

	// ホイール値の設定
	void SetWheel(int wheel) { this->wheel[0] += wheel; }

	// ホイール値の取得
	int GetWheel() const { return wheel[1]; }

	// マウスカーソルX座標取得
	int GetPositionX() const { return position_x[0]; }

	// マウスカーソルY座標取得
	int GetPositionY() const { return position_y[0]; }

	// 前回のマウスカーソルX座標取得
	int GetOldPositionX() const { return position_x[1]; }

	// 前回のマウスカーソルY座標取得
	int GetOldPositionY() const { return position_y[1]; }

	// スクリーン幅設定
	void SetScreenWidth(int width) { screen_width = width; }

	// スクリーン高さ設定
	void SetScreenHeight(int height) { screen_height = height; }

	// スクリーン幅取得
	int GetScreenWidth() const { return screen_width; }

	// スクリーン高さ取得
	int GetScreenHeight() const { return screen_height; }

private:
	MouseButton		button_state[2] = { 0 };
	MouseButton		button_down = 0;
	MouseButton		button_up = 0;
	int				position_x[2];
	int				position_y[2];
	int				wheel[2];
	int				screen_width = 0;
	int				screen_height = 0;
	HWND			hWnd = nullptr;
};
