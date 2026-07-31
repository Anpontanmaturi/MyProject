#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "misc.h"

#ifdef USE_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

class Graphics
{
public:

    Graphics(HWND hwnd);
    ~Graphics();

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    Graphics(Graphics&&) noexcept = delete;
    Graphics& operator=(Graphics&&) noexcept = delete;

    // インスタンス取得
    static Graphics& Instance() { return *instance; }

    void BeginFrame();
    void EndFrame();

    ID3D11Device* GetDevice()const { return device.Get(); }
    ID3D11DeviceContext* GetContext()const { return immediate_context.Get(); }
    IDXGISwapChain* GetSwapChain()const { return swap_chain.Get();}

private:
    void CreateDevice(HWND hwnd);
    void CreateRenderTarget();

private:
    static Graphics* instance;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;

    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

    float	screen_width = {};
    float	screen_height = {};
};