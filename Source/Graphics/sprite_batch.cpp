#include "sprite_batch.h"
#include "System/misc.h"
#include <sstream>
#include <WICTextureLoader.h>
#include "texture.h"
#include "shader.h"

// COMオブジェクトを ComPtrスマートポインターテンプレートを使った変数宣言に変更
SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites)
	: max_vertices(max_sprites * 6)
{
	HRESULT hr{ S_OK };

	//頂点バッファオブジェクトの生成 
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(vertex) * max_vertices;
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	
	hr = device->CreateBuffer(&buffer_desc, NULL, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//入力レイアウトオブジェクトの生成 
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// 頂点シェーダーファイルロードのコード
	hr = create_vs_from_cso(device, "Shader\\sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(),
		input_element_desc, _countof(input_element_desc));

	// ピクセルシェーダーファイルロードのコード
	hr = create_ps_from_cso(device, "Shader\\sprite_ps.cso", pixel_shader.GetAddressOf());

	// 画像ファイルのロードとテクスチャ情報の取得
	hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void SpriteBatch::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh)
{
	render(immediate_context, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
}

void SpriteBatch::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle/*degree*/
)
{
	float tex_width = static_cast<float>(texture2d_desc.Width);
	float tex_height = static_cast<float>(texture2d_desc.Height);

	this->render(immediate_context,
		dx, dy, dw, dh,
		r, g, b, a,
		angle,
		0.0f, 0.0f, static_cast<float>(tex_width), static_cast<float>(tex_height));
}

void SpriteBatch::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle/*degree*/,
	float sx, float sy, float sw, float sh
)
{
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	////left-top
	//float x0{ dx };
	//float y0{ dy };
	////right-top
	//float x1{ dx + dw };
	//float y1{ dy };
	////left-bottom
	//float x2{ dx };
	//float y2{ dy + dh };
	////right-bottom
	//float x3{ dx + dw };
	//float y3{ dy + dh };
	//
	//auto rotate = [](float& x, float& y, float cx, float cy, float angle)
	//	{
	//		x -= cx;
	//		y -= cy;
	//
	//		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	//		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	//		float tx{ x }, ty{ y };
	//		x = cos * tx + -sin * ty;
	//		y = sin * tx + cos * ty;
	//
	//		x += cx;
	//		y += cy;
	//	};
	////回転の中心を矩形の中心点にした場合
	//float cx = dx + dw * 0.5f;
	//float cy = dy + dh * 0.5f;
	//rotate(x0, y0, cx, cy, angle);
	//rotate(x1, y1, cx, cy, angle);
	//rotate(x2, y2, cx, cy, angle);
	//rotate(x3, y3, cx, cy, angle);
	//
	//
	////スクリーン座標系からNDCへの座標変換をおこなう
	//x0 = 2.0f * x0 / viewport.Width - 1.0f;
	//y0 = 1.0f - 2.0f * y0 / viewport.Height;
	//x1 = 2.0f * x1 / viewport.Width - 1.0f;
	//y1 = 1.0f - 2.0f * y1 / viewport.Height;
	//x2 = 2.0f * x2 / viewport.Width - 1.0f;
	//y2 = 1.0f - 2.0f * y2 / viewport.Height;
	//x3 = 2.0f * x3 / viewport.Width - 1.0;
	//y3 = 1.0f - 2.0f * y3 / viewport.Height;
	//
	//// 09:変更
	//float u0{ sx / texture2d_desc.Width };
	//float v0{ sy / texture2d_desc.Height };
	//float u1{ (sx + sw) / texture2d_desc.Width };
	//float v1{ (sy + sh) / texture2d_desc.Height };
	//
	//vertices.push_back({ { x0, y0, 0 },{ r, g, b, a },{ u0, v0 } });
	//vertices.push_back({ { x1, y1, 0 },{ r, g, b, a },{ u1, v0 } });
	//vertices.push_back({ { x2, y2, 0 },{ r, g, b, a },{ u0, v1 } });
	//vertices.push_back({ { x2, y2, 0 },{ r, g, b, a },{ u0, v1 } });
	//vertices.push_back({ { x1, y1, 0 },{ r, g, b, a },{ u1, v0 } });
	//vertices.push_back({ { x3, y3, 0 },{ r, g, b, a },{ u1, v1 } });

	// 回転の中心
	const float cx = dx + dw * 0.5f;
	const float cy = dy + dh * 0.5f;

	// 角度のラジアン変換 + cos/sinの一度きりの計算
	const float rad = DirectX::XMConvertToRadians(angle);
	const float cos_a = cosf(rad);
	const float sin_a = sinf(rad);

	// 各頂点のスクリーン座標（左上、右上、左下、右下）
	float positions[4][2] = {
		{ dx        , dy        },  // 左上
		{ dx + dw   , dy        },  // 右上
		{ dx        , dy + dh   },  // 左下
		{ dx + dw   , dy + dh   }   // 右下
	};

	// 回転適用
	for (int i = 0; i < 4; ++i)
	{
		float& x = positions[i][0];
		float& y = positions[i][1];
		float tx = x - cx;
		float ty = y - cy;
		x = cos_a * tx - sin_a * ty + cx;
		y = sin_a * tx + cos_a * ty + cy;
	}

	// スクリーン座標 → NDC 変換
	for (int i = 0; i < 4; ++i)
	{
		positions[i][0] = 2.0f * positions[i][0] / viewport.Width - 1.0f;
		positions[i][1] = 1.0f - 2.0f * positions[i][1] / viewport.Height;
	}

	// テクスチャ座標計算
	const float u0 = sx / texture2d_desc.Width;
	const float v0 = sy / texture2d_desc.Height;
	const float u1 = (sx + sw) / texture2d_desc.Width;
	const float v1 = (sy + sh) / texture2d_desc.Height;

	// 頂点データを追加（6頂点分）
	vertices.push_back({ { positions[0][0], positions[0][1], 0 }, { r, g, b, a }, { u0, v0 } }); // 左上
	vertices.push_back({ { positions[1][0], positions[1][1], 0 }, { r, g, b, a }, { u1, v0 } }); // 右上
	vertices.push_back({ { positions[2][0], positions[2][1], 0 }, { r, g, b, a }, { u0, v1 } }); // 左下

	vertices.push_back({ { positions[2][0], positions[2][1], 0 }, { r, g, b, a }, { u0, v1 } }); // 左下
	vertices.push_back({ { positions[1][0], positions[1][1], 0 }, { r, g, b, a }, { u1, v0 } }); // 右上
	vertices.push_back({ { positions[3][0], positions[3][1], 0 }, { r, g, b, a }, { u1, v1 } }); // 右下

	////計算結果で頂点バッファオブジェクトを更新する
	//HRESULT hr{ S_OK };
	//D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	//hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
	//	&mapped_subresource);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//
	//vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	//if (vertices != nullptr)
	//{
	//	vertices[0].position = { x0,y0,0 };
	//	vertices[1].position = { x1,y1,0 };
	//	vertices[2].position = { x2,y2,0 };
	//	vertices[3].position = { x3,y3,0 };
	//	//vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1,1,1,1 };
	////テクスチャ座標を頂点バッファにセットする 
	//	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };
	//
	//	float tex_width = static_cast<float>(texture2d_desc.Width);
	//	float tex_height = static_cast<float>(texture2d_desc.Height);
	//
	//	float u0 = sx / tex_width;
	//	float v0 = sy / tex_height;
	//	float u1 = (sx + sw) / tex_width;
	//	float v1 = (sy + sh) / tex_height;
	//
	//	vertices[0].texcoord = { u0, v0 };
	//	vertices[1].texcoord = { u1, v0 };
	//	vertices[2].texcoord = { u0, v1 };
	//	vertices[3].texcoord = { u1, v1 };
	//}
	//
	//immediate_context->Unmap(vertex_buffer, 0);
	//
	//
	//
	////頂点バッファーのバインド 
	//UINT stride{ sizeof(vertex) };
	//UINT offset{ 0 };
	//immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	//
	////プリミティブタイプおよびデータの順序に関する情報のバインド 
	//immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//
	////入力レイアウトオブジェクトのバインド
	//immediate_context->IASetInputLayout(input_layout);
	//
	////シェーダーのバインド
	//immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	//immediate_context->PSSetShader(pixel_shader, nullptr, 0);
	//
	////シェーダー リソースのバインド
	//immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);
	//
	////プリミティブの描画 
	//immediate_context->Draw(4, 0);
}

// 09:beginメンバ関数の実装
void SpriteBatch::begin(ID3D11DeviceContext* immediate_context)
{
	vertices.clear();
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
}

// endメンバ関数の実装
void SpriteBatch::end(ID3D11DeviceContext* immediate_context)
{
	//計算結果で頂点バッファオブジェクトを更新する
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
	vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (data != nullptr)
	{
		const vertex* p = vertices.data();
		memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);

	//頂点バッファーのバインド 
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//プリミティブタイプおよびデータの順序に関する情報のバインド 
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());

	////シェーダーのバインド
	//immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	//immediate_context->PSSetShader(pixel_shader, nullptr, 0);
	//
	////シェーダー リソースのバインド
	//immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	//プリミティブの描画 
	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);
}

SpriteBatch::~SpriteBatch()
{
	/*vertex_shader->Release();
	pixel_shader->Release();
	input_layout->Release();
	vertex_buffer->Release();

	shader_resource_view->Release();*/

	release_all_textures();
}
