#pragma once

using GamePadButtons = unsigned int;

class GamePad
{
public:
	static const GamePadButtons BTN_UP = (1 << 0);
	static const GamePadButtons BTN_RIGHT = (1 << 1);
	static const GamePadButtons BTN_DOWN = (1 << 2);
	static const GamePadButtons BTN_LEFT = (1 << 3);
	static const GamePadButtons BTN_A = (1 << 4);
	static const GamePadButtons BTN_B = (1 << 5);
	static const GamePadButtons BTN_X = (1 << 6);
	static const GamePadButtons BTN_Y = (1 << 7);
	static const GamePadButtons BTN_START = (1 << 8);
	static const GamePadButtons BTN_BACK = (1 << 9);
	static const GamePadButtons BTN_LEFT_THUMB = (1 << 10);
	static const GamePadButtons BTN_RIGHT_THUMB = (1 << 11);
	static const GamePadButtons BTN_LEFT_SHOULDER = (1 << 12);
	static const GamePadButtons BTN_RIGHT_SHOULDER = (1 << 13);
	static const GamePadButtons BTN_LEFT_TRIGGER = (1 << 14);
	static const GamePadButtons BTN_RIGHT_TRIGGER = (1 << 15);

public:
	GamePad() {}
	~GamePad() {}

	static GamePad& Instance()
	{
		static GamePad instance;
		return instance;
	}

	void update();

	// スロット設定
	void set_slot(int slot) { this->slot = slot; }

	// ボタン入力状態の取得
	GamePadButtons get_button() const { return button_state[0]; }

	// ボタン押下状態の取得
	GamePadButtons get_button_down() const { return button_down; }

	// ボタン押上状態の取得
	GamePadButtons get_button_up() const { return button_up; }

	// 左スティックX軸入力状態の取得
	float get_axis_lx() const { return axis_lx; }

	// 左スティックY軸入力状態の取得
	float get_axis_ly() const { return axis_ly; }

	// 右スティックX軸入力状態の取得
	float get_axis_rx() const { return axis_rx; }

	// 右スティックY軸入力状態の取得
	float get_axis_ry() const { return axis_ry; }

	// 左トリガー入力状態の取得
	float get_trigger_l() const { return trigger_l; }

	// 右トリガー入力状態の取得
	float get_trigger_r() const { return trigger_r; }

private:
	GamePadButtons		button_state[2] = { 0 };
	GamePadButtons		button_down = 0;
	GamePadButtons		button_up = 0;
	float				axis_lx = 0.0f;
	float				axis_ly = 0.0f;
	float				axis_rx = 0.0f;
	float				axis_ry = 0.0f;

	float				trigger_l = 0.0f;
	float				trigger_r = 0.0f;

	int					slot = 0;
};