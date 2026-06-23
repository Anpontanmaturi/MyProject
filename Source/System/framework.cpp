#include "framework.h"
#include "Graphics/shader.h"
#include "Input/game_pad.h"

Framework::Framework(HWND hwnd) : hwnd(hwnd)
{
}

bool Framework::Initialize()
{
	graphics_system = std::make_unique<Graphics>(hwnd);

	return true;
}

void Framework::Update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	GamePad::Instance().Update();

#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	Graphics::Instance().Update(elapsed_time);
}

void Framework::Render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	if (graphics_system)
	{
		graphics_system->Render(elapsed_time);
	}
}

bool Framework::Uninitialize()
{
	return true;
}

Framework::~Framework()
{

}