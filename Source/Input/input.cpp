#include "input.h"

Input* Input::instance = nullptr;

// コンストラクタ
Input::Input(HWND hwnd) : mouse(hwnd)
{
	instance = this;
}

// 更新処理
void Input::update()
{
	gamepad.update();
	mouse.update();
}