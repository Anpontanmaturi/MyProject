#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <wrl.h>

class SpriteBatch
{
public:

	D3D11_TEXTURE2D_DESC texture2d_desc;	

	// COMオブジェクトを ComPtrスマートポインターテンプレートを使った変数宣言に変更
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

	void Render(ID3D11DeviceContext* immediate_context,
		float dx, float dy,			//矩形の左上の座標（スクリーン座標系）
		float dw, float dh,			//矩形のサイズ（スクリーン座標系
		float r, float g, float b, float a,
		float angle/*degree*/
	);
	void Render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle/*degree*/,
		float sx, float sy, float sw, float sh
	);
	void Render(ID3D11DeviceContext* immediate_context, 
		float dx, float dy, float dw, float dh);

	void Begin(ID3D11DeviceContext* immediate_context);
	void End(ID3D11DeviceContext* immediate_context);

	SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites);
	~SpriteBatch();

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	const size_t max_vertices;
	std::vector<vertex> vertices;

};
