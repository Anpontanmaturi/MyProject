#include "graphics.h"

Graphics* Graphics::instance = nullptr;

Graphics::Graphics(HWND hwnd)
{
	CreateDevice(hwnd);
	CreateRenderTarget();
}

Graphics::~Graphics()
{
	instance = nullptr;
}

void Graphics::CreateDevice(HWND hwnd)
{
	_ASSERT_EXPR(instance == nullptr, L"already instantiated");
	instance = this;

	// 画面のサイズを取得
	RECT rect{};
	GetClientRect(hwnd, &rect);
	UINT client_width = rect.right - rect.left;
	UINT client_height = rect.bottom - rect.top;

	this->screen_width = static_cast<float>(client_width);
	this->screen_height = static_cast<float>(client_height);
	HRESULT hr{ S_OK };

	// デバイス・デバイスコンテキスト・スワップチェーンの作成
	UINT create_device_flags{ 0 };
#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };
	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Width = static_cast<UINT>(screen_width);
	swap_chain_desc.BufferDesc.Height = static_cast<UINT>(screen_height);
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, create_device_flags, &feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
		swap_chain.GetAddressOf(), device.GetAddressOf(), NULL, immediate_context.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Graphics::CreateRenderTarget()
{
	HRESULT hr{ S_OK };

	// レンダーターゲットビュー
	ID3D11Texture2D* back_buffer{};
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(back_buffer, NULL, render_target_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	back_buffer->Release();

	// 深度ステンシルビュー
	ID3D11Texture2D* depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = static_cast<UINT>(this->screen_width);
	texture2d_desc.Height = static_cast<UINT>(this->screen_height);
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, NULL, &depth_stencil_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = texture2d_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depth_stencil_buffer,
		&depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_buffer->Release();

	// ビューポートの設定
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = this->screen_width;
	viewport.Height = this->screen_height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);
}

void Graphics::BeginFrame()
{
	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
}

void Graphics::EndFrame()
{
	UINT sync_interval{ 0 };
	swap_chain->Present(sync_interval, 0);
}
