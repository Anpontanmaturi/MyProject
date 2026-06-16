#pragma once

#include <Windows.h>
#include "game_pad.h"

// 入力
class Input
{
public:
	Input(HWND hwnd);
	~Input() {}
	
	// インスタンス取得
	static Input& Instance() { return *instance; }

	// 更新
	void update();

	// ゲームパッド取得
	GamePad& get_game_pad() { return gamepad; }

private:
	static Input* instance;
	GamePad				gamepad;
	//Mouse				mouse;
};