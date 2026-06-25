#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <directxmath.h>

#include "System/misc.h"

#ifdef USE_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

// 必要なアセットヘッダーをここに集約
#include "skinned_mesh.h"
#include "Stage/stage.h"
#include "Character/Player.h"

#include "Camera/camera_controller.h"

// グラフィックス
class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	void Render(float elapsed_time);
	void Update(float elapsed_time);

	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	Graphics(Graphics&&) noexcept = delete;
	Graphics& operator=(Graphics&&) noexcept = delete;

	// インスタンス取得
	static Graphics& Instance() { return *instance; }

	// 構造体の宣言
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	// 各種のパイプラインステートオブジェクトを配列で宣言
	enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE};
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[5];

	enum class DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF };
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];

	enum class BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY }; 
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];

	enum class RASTER_STATE { SOLID, WIREFRAME, CULL_NONE, WIREFRAME_CULL_NONE };
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[4];

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];
	
	// シェーダー定数構造体
	struct scene_constants
	{
		DirectX::XMFLOAT4X4 view_projection;	//ビュー・プロジェクション変換行列
		DirectX::XMFLOAT4 light_direction;		//ライトの向き
		DirectX::XMFLOAT4 camera_position;
	};
	struct parameter_constants
	{
		float luminance_threshold{ 0.8f };
		float gaussian_sigma{ 1.0f };
		float bloom_intensity{ 1.0f };
		float exposure{ 1.2f };
	};
	parameter_constants parameter_constants;

	std::unique_ptr<Stage> stage;
	std::unique_ptr<Player> player;

	CameraController camera_controller;

private:
	static Graphics* instance;

	float gltf_scale = 1.0f;
	float factors[3] = {};

	DirectX::XMFLOAT4 light_direction = { 0.0f,-1.0f,0.0f,0.0f };

	float	screen_width = {};
	float	screen_height = {};

	DirectX::XMFLOAT3 player_pos = {};
	DirectX::XMFLOAT3 player_scale = {};

};