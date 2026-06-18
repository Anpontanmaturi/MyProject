#pragma once

#include <Windows.h>
#include "game_pad.h"
#include "mouse.h"

// 入力
class Input
{
public:
	Input(HWND hwnd);
	~Input() {}
	
	// インスタンス取得
	static Input& Instance() { return *instance; }

	// 更新
	void Update();

	// ゲームパッド取得
	GamePad& GetGamePad() { return gamepad; }

	// マウス取得
	Mouse& GetMouse() { return mouse; }

private:
	static Input* instance;
	GamePad				gamepad;
	Mouse				mouse;
};