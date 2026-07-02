#include "graphics.h"
#include "Graphics/shader.h"

#include "texture.h"
#include "Camera/camera.h"

#include <cassert>

Graphics* Graphics::instance = nullptr;

#ifndef _ASSERT_EXPR
#define _ASSERT_EXPR(expression, message) assert(expression)
#endif

Graphics::Graphics(HWND hWnd)
{
	_ASSERT_EXPR(instance == nullptr, L"already instantiated");
	instance = this;

	// 画面のサイズを取得
	RECT rect{};
	GetClientRect(hWnd, &rect);
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
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = static_cast<UINT>(screen_width);
	swap_chain_desc.BufferDesc.Height = static_cast<UINT>(screen_height);
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hWnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
		&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
		swap_chain.GetAddressOf(), device.GetAddressOf(), NULL, immediate_context.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	// レンダーターゲットビューの作成
	ID3D11Texture2D* back_buffer{};
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(back_buffer, NULL, render_target_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	back_buffer->Release();


	// 深度ステンシルビューの作成
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


	// サンプラーステートオブジェクトの作成
	D3D11_SAMPLER_DESC sampler_desc{};
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());// POINT
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = device->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());// LINEAR
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = device->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());// ANISOTROPIC
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// LINEAR_BORDER_BLACK
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	hr = device->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// LINEAR_BORDER_WHITE
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.BorderColor[0] = 1;
	sampler_desc.BorderColor[1] = 1;
	sampler_desc.BorderColor[2] = 1;
	sampler_desc.BorderColor[3] = 1;
	hr = device->CreateSamplerState(&sampler_desc, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	// 深度ステンシルステートオブジェクトの作成
	//深度テストON、深度ライトON
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//深度テストON、深度ライトOFF
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//深度テストOFF、深度ライトON
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_ON)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//深度テストOFF、深度ライトOFF
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	// ブレンディングステートオブジェクトを作成
	D3D11_BLEND_DESC blend_desc{};
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = FALSE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::NONE)].GetAddressOf());// 無効
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].GetAddressOf());// 加算
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::ADD)].GetAddressOf());// 減算
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, blend_states[static_cast<size_t>(BLEND_STATE::MULTIPLY)].GetAddressOf());// 乗算
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	// ラスタライザステートオブジェクトを生成
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = TRUE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[static_cast<size_t>(RASTER_STATE::WIREFRAME)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[static_cast<size_t>(RASTER_STATE::WIREFRAME_CULL_NONE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// シーン定数バッファオブジェクトを生成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(scene_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = sizeof(parameter_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	stage = std::make_unique<Stage>(device.Get());
	player = std::make_unique<Player>(device.Get());
	player->SetScale({ 0.01f, 0.01f, 0.01f });
	sandbag = std::make_unique<Sandbag>(device.Get());
	sandbag->SetScale({ 0.01f, 0.01f, 0.01f });

	Camera& camera = Camera::Instance();
	camera.SetLookat(
		{ 10, 10, -10 },
		{ 10, 0, 0 },
		{ 0, 1, 0 });
	camera.SetPerspecticeFov(DirectX::XM_PIDIV4, viewport.Width / viewport.Height, 0.1f, 1000.0f);
}

Graphics::~Graphics()
{
	if (instance == this)
	{
		instance = nullptr;
	}
}

void Graphics::Update(float elapsed_time)
{
	stage->UpdateTransform();
	player->Update(elapsed_time);
	sandbag->Update(elapsed_time);

	{
		camera_controller.SetTarget(player->GetCameraLookAt(), player->GetRotation());
		camera_controller.SetMove(player->MoveOtherBack());
		camera_controller.Update(elapsed_time);
	}
#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
	{
		if (ImGui::CollapsingHeader("light", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::SliderFloat("light_direction.x", &light_direction.x, -1.0f, +1.0f);
			ImGui::SliderFloat("light_direction.y", &light_direction.y, -1.0f, +1.0f);
			ImGui::SliderFloat("light_direction.z", &light_direction.z, -1.0f, +1.0f);
		}
		if (ImGui::CollapsingHeader("player", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			player_pos = player->GetPosition();
			player_scale = player->GetScale();
			ImGui::InputFloat3("player_position", &player_pos.x);
			ImGui::InputFloat3("player_position", &player_scale.x);
			ImGui::Text("move other front: %s", player->MoveOtherBack() ? "true" : "false"); 
		}
	}
	ImGui::End();
#endif
}

void Graphics::Render(float elapsed_time)
{
	HRESULT hr{ S_OK };

	FLOAT color[]{ 0.2f,0.2f,0.2f,1.0f };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	immediate_context->PSSetSamplers(0, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
	immediate_context->PSSetSamplers(1, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
	immediate_context->PSSetSamplers(2, 1, sampler_states[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());

	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& camera_position = camera.GetEye();
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera.GetView());
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera.GetProjection());

	scene_constants data{};
	DirectX::XMStoreFloat4x4(&data.view_projection, V * P);
	data.light_direction = light_direction;
	data.camera_position.x = camera_position.x;
	data.camera_position.y = camera_position.y;
	data.camera_position.z = camera_position.z;
	data.camera_position.w = 0;
	immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
	immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

	// 3Dオブジェクトの描画
	stage->Render(immediate_context.Get());
	player->Render(immediate_context.Get());
	sandbag->Render(immediate_context.Get());

#ifdef USE_IMGUI
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	UINT sync_interval{ 0 };
	swap_chain->Present(sync_interval, 0);

}