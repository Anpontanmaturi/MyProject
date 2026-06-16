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

	void update();

	// ボタンの状態を取得
	MouseButtons get_button() const { return button_state[0]; } // 入力
	MouseButtons get_button_down() const { return button_down; } // 押下
	MouseButtons get_button_up() const { return button_up; }		// 押上

	// マウスホイール値の設定・取得
	void set_wheel(int wheel) { this->wheel[0] += wheel; }
	int get_wheel()const { return wheel[1]; }

	// マウスカーソル座標取得
	int get_position_x() const { return position_x[0]; }
	int get_position_y() const { return position_y[0]; }
	// 前回の座標
	int get_old_position_x() const { return position_x[1]; }
	int get_old_position_y() const { return position_y[1]; }

	// スクリーン系の設定・取得
	void set_screen_width(int width) { screen_width = width; }
	void set_screen_height(int height) { screen_height = height; }
	int get_screen_width() const { return screen_width; }
	int get_screen_height() const { return screen_height; }

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
