#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "misc.h"
#include "high_resolution_timer.h"
#include "Graphics/graphics.h"

#include "framebuffer.h"
#include "Graphics/fullscreen_quad.h"
#include "Graphics/gltf_model.h"

//#include "Camera/camera.h"
//#include "Camera/free_camera_controller.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FULLSCREEN FALSE
#define APPLICATION_NAME L"X3DGP"

class Framework
{
public:
	CONST HWND hwnd;

	std::unique_ptr<Graphics> graphics_system;

	Framework(HWND hwnd);
	~Framework();

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

	int run()
	{
		MSG msg{};

#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(graphics_system->device.Get(), graphics_system->immediate_context.Get());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();
				update(tictoc.time_interval());
				render(tictoc.time_interval());
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

#if 1
		BOOL fullscreen = 0;
		graphics_system->swap_chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			graphics_system->swap_chain->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool uninitialize();

	DirectX::XMFLOAT4 camera_position = { 0.0f,0.0f,-10.0f,1.0f };
	DirectX::XMFLOAT4 light_direction = { 0.0f,0.0f,1.0f,1.0f };

	//camera camera;
	//free_camera_controller camera_controller;


private:
	HighResolutionTimer tictoc;
	uint32_t frames_per_second{ 0 };
	float count_by_seconds{ 0.0f };
	void calculate_frame_stats()
	{
		if (++frames_per_second, (tictoc.time_stamp() - count_by_seconds) >= 1.0f)
		{
			float fps = static_cast<float>(frames_per_second);
			std::wostringstream outs;
			outs.precision(6);
			outs << L"X3DGP" << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames_per_second = 0;
			count_by_seconds += 1.0f;
		}
	}
};

