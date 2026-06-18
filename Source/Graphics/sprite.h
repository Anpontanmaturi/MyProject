#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <string>

class Sprite
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
	void Render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);
	
	Sprite(ID3D11Device *device, const wchar_t* filename);
	~Sprite();
	
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};	

	// フォント画像ファイルを使用し任意の文字列を画面に出力する機能を追加する
	void Textout(ID3D11DeviceContext* immediate_context, std::string s,
		float x, float y, float w, float h, float r, float g, float b, float a);
};

