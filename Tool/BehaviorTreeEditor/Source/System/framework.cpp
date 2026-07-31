#include "framework.h"

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
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

	editor.Update(elapsed_time);
}

void Framework::Render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	graphics_system->BeginFrame();

	editor.Draw();

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	graphics_system->EndFrame();
}

bool Framework::Uninitialize()
{
	return true;
}

Framework::~Framework()
{

}