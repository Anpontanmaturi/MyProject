#include "mouse.h"

static const int KeyMap[] =
{
	VK_LBUTTON, // 左ボタン
	VK_MBUTTON, // 中ボタン
	VK_RBUTTON, // 右ボタン
};

// コンストラクタ
Mouse::Mouse(HWND hWnd) : hWnd(hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	screen_width = rc.right - rc.left;
	screen_height = rc.bottom - rc.top;
}

// 更新
void Mouse::update()
{
	// スイッチ情報
	MouseButtons new_button_state = 0;

	for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
	{
		if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
		{
			new_button_state |= (1 << i);
		}
	}

	// ホイール
	wheel[1] = wheel[0];
	wheel[0] = 0;

	// ボタン情報更新
	button_state[1] = button_state[0];
	button_state[0] = new_button_state;

	button_down = ~button_state[1] & new_button_state;
	button_up = ~new_button_state & button_state[1];

	// カーソル位置の取得
	POINT cursor;
	::GetCursorPos(&cursor);
	::ScreenToClient(hWnd, &cursor);

	// 画面サイズの取得
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screen_w = rc.right - rc.left;
	UINT screen_h = rc.bottom - rc.top;
	UINT viewport_w = screen_w;
	UINT viewport_h = screen_h;

	// 画面補正
	position_x[1] = position_x[0];
	position_y[1] = position_y[0];
	position_x[0] = (LONG)(cursor.x / static_cast<float>(viewport_w) * static_cast<float>(screen_w));
	position_y[0] = (LONG)(cursor.y / static_cast<float>(viewport_h) * static_cast<float>(screen_h));
}