#include "framework.h"
#include "Graphics/shader.h"

Framework::Framework(HWND hwnd) : hwnd(hwnd)
{
	graphics_system = std::make_unique<Graphics>(hwnd);
}

void Framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void Framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	if (graphics_system)
	{
		graphics_system->render(elapsed_time, camera_position);
	}
}

bool Framework::uninitialize()
{
	return true;
}

Framework::~Framework()
{

}