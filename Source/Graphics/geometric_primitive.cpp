#include "geometric_primitive.h"
#include "shader.h"
#include "System/misc.h"

GeometricPrimitive::GeometricPrimitive(ID3D11Device* device)
{
	//create_cube(device);
	//create_cylinder(device);
	//create_sphere(device);
	//create_capsule(device);

	HRESULT hr{ S_OK };

	D3D11_INPUT_ELEMENT_DESC  input_element_desc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	create_vs_from_cso(device, "Shader\\geometric_primitive_vs.cso", vertex_shader.GetAddressOf(),
		input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, "Shader\\geometric_primitive_ps.cso", pixel_shader.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void GeometricPrimitive::create_com_buffers(ID3D11Device* device, vertex* vertices,
	size_t vertex_count, uint32_t* indices, size_t index_count)
{
	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	D3D11_SUBRESOURCE_DATA subresource_data{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void GeometricPrimitive::render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	constants data{ world, material_color };
	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

void GeometricPrimitive::create_cube(ID3D11Device* device) {
	vertex vertices[24]{};
	// サイズが 1.0 の正立方体データを作成する（重心を原点にする）。正立方体のコントロールポイント数は 8 個、
	// 1 つのコントロールポイントの位置には法線の向きが違う頂点が 3 個あるので頂点情報の総数は 8x3=24 個、
	// 頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。

	//前面
	vertices[0].position = { -0.5f,-0.5f,-0.5f };
	vertices[0].normal = { 0,0,-1 };
	vertices[1].position = { -0.5f,0.5f,-0.5f };
	vertices[1].normal = { 0,0,-1 };
	vertices[2].position = { 0.5f,0.5f,-0.5f };
	vertices[2].normal = { 0,0,-1 };
	vertices[3].position = { 0.5f,-0.5f,-0.5f };
	vertices[3].normal = { 0,0,-1 };
	//右側面
	vertices[4].position = { 0.5f,-0.5f,-0.5f };
	vertices[4].normal = { 1,0,0 };
	vertices[5].position = { 0.5f,0.5f,-0.5f };
	vertices[5].normal = { 1,0,0 };
	vertices[6].position = { 0.5f,0.5f,0.5f };
	vertices[6].normal = { 1,0,0 };
	vertices[7].position = { 0.5f,-0.5f,0.5f };
	vertices[7].normal = { 1,0,0 };
	//裏面
	vertices[8].position = { 0.5f,-0.5f,0.5f };
	vertices[8].normal = { 0,0,1 };
	vertices[9].position = { 0.5f,0.5f,0.5f };
	vertices[9].normal = { 0,0,1 };
	vertices[10].position = { -0.5f,0.5f,0.5f };
	vertices[10].normal = { 0,0,1 };
	vertices[11].position = { -0.5f,-0.5f,0.5f };
	vertices[11].normal = { 0,0,1 };
	//左側面
	vertices[12].position = { -0.5f,-0.5f,0.5f };
	vertices[12].normal = { -1,0,0 };
	vertices[13].position = { -0.5f,0.5f,0.5f };
	vertices[13].normal = { -1,0,0 };
	vertices[14].position = { -0.5f,0.5f,-0.5f };
	vertices[14].normal = { -1,0,0 };
	vertices[15].position = { -0.5f,-0.5f,-0.5f };
	vertices[15].normal = { -1,0,0 };
	//上面
	vertices[16].position = { -0.5f,0.5f,-0.5f };
	vertices[16].normal = { 0,1,0 };
	vertices[17].position = { -0.5f,0.5f,0.5f };
	vertices[17].normal = { 0,1,0 };
	vertices[18].position = { 0.5f,0.5f,0.5f };
	vertices[18].normal = { 0,1,0 };
	vertices[19].position = { 0.5f,0.5f,-0.5f };
	vertices[19].normal = { 0,1,0 };
	//下面
	vertices[20].position = { 0.5f,-0.5f,-0.5f };
	vertices[20].normal = { 0,-1,0 };
	vertices[21].position = { 0.5f,-0.5f,0.5f };
	vertices[21].normal = { 0,-1,0 };
	vertices[22].position = { -0.5f,-0.5f,0.5f };
	vertices[22].normal = { 0,-1,0 };
	vertices[23].position = { -0.5f,-0.5f,-0.5f };
	vertices[23].normal = { 0,-1,0 };

	uint32_t indices[36]{};

	//表面
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;
	//右側面
	indices[6] = 4;
	indices[7] = 5;
	indices[8] = 6;

	indices[9] = 4;
	indices[10] = 6;
	indices[11] = 7;
	//裏面
	indices[12] = 8;
	indices[13] = 9;
	indices[14] = 10;

	indices[15] = 8;
	indices[16] = 10;
	indices[17] = 11;
	//左側面
	indices[18] = 12;
	indices[19] = 13;
	indices[20] = 14;

	indices[21] = 12;
	indices[22] = 14;
	indices[23] = 15;
	//上面
	indices[24] = 16;
	indices[25] = 17;
	indices[26] = 18;

	indices[27] = 16;
	indices[28] = 18;
	indices[29] = 19;
	//下面
	indices[30] = 20;
	indices[31] = 21;
	indices[32] = 22;

	indices[33] = 20;
	indices[34] = 22;
	indices[35] = 23;

	create_com_buffers(device, vertices, 24, indices, 36);
}

void GeometricPrimitive::create_cylinder(ID3D11Device* device) {
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

	float height = 1.5f;
	int slice_count = 20;
	float radius = 0.5f;

	float half_height = height * 0.5f;
	float delta_theta = DirectX::XM_2PI / slice_count;

	// 側面
	for (int i = 0; i <= slice_count; ++i) {
		float theta = i * delta_theta;
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		DirectX::XMFLOAT3 normal = { cosf(theta), 0.0f, sinf(theta) };

		vertices.push_back({ {x, -half_height, z}, normal });
		vertices.push_back({ {x, +half_height, z}, normal });
	}

	for (int i = 0; i < slice_count; ++i) {
		indices.push_back(i * 2);
		indices.push_back(i * 2 + 1);
		indices.push_back(i * 2 + 3);

		indices.push_back(i * 2);
		indices.push_back(i * 2 + 3);
		indices.push_back(i * 2 + 2);
	}

	// ===== 上面 =====
	uint32_t top_center_index = static_cast<uint32_t>(vertices.size());
	vertices.push_back({ {0.0f, +half_height, 0.0f}, {0.0f, 1.0f, 0.0f} });  // 中心

	for (int i = 0; i <= slice_count; ++i) {
		float theta = i * delta_theta;
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		vertices.push_back({ {x, +half_height, z}, {0.0f, 1.0f, 0.0f} });  // 周辺
	}

	for (int i = 0; i < slice_count; ++i) {
		indices.push_back(top_center_index + i + 2);
		indices.push_back(top_center_index + i + 1);
		indices.push_back(top_center_index);
	}

	// ===== 下面 =====
	uint32_t bottom_center_index = static_cast<uint32_t>(vertices.size());
	vertices.push_back({ {0.0f, -half_height, 0.0f}, {0.0f, -1.0f, 0.0f} });  // 中心

	for (int i = 0; i <= slice_count; ++i) {
		float theta = i * delta_theta;
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		vertices.push_back({ {x, -half_height, z}, {0.0f, -1.0f, 0.0f} });  // 周辺
	}

	for (int i = 0; i < slice_count; ++i) {
		indices.push_back(bottom_center_index + i + 1);
		indices.push_back(bottom_center_index + i + 2);
		indices.push_back(bottom_center_index);
	}


	create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void GeometricPrimitive::create_sphere(ID3D11Device* device)
{
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

	float radius = 0.5f;
	int slice_count = 20;
	int stack_count = 20;

	for (int stack = 0; stack <= stack_count; ++stack) {
		float phi = DirectX::XM_PI * stack / stack_count;
		for (int slice = 0; slice <= slice_count; ++slice) {
			float theta = DirectX::XM_2PI * slice / slice_count;

			float x = sinf(phi) * cosf(theta);
			float y = cosf(phi);
			float z = sinf(phi) * sinf(theta);

			vertices.push_back({ {radius * x, radius * y, radius * z}, {x, y, z} });
		}
	}

	for (int stack = 0; stack < stack_count; ++stack) {
		for (int slice = 0; slice < slice_count; ++slice) {
			uint32_t a = stack * (slice_count + 1) + slice;
			uint32_t b = a + slice_count + 1;

			indices.push_back(b);
			indices.push_back(a);
			indices.push_back(a + 1);

			indices.push_back(a + 1);
			indices.push_back(b + 1);
			indices.push_back(b);
		}
	}

	create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void GeometricPrimitive::create_capsule(ID3D11Device* device)
{
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

	float radius = 0.5f;
	float height = 1.5f;
	int slice_count = 20;
	int stack_count = 20;

	float half_height = height * 0.5f;
	float delta_theta = DirectX::XM_2PI / slice_count;

	// === 円柱側面 ===
	for (int i = 0; i <= slice_count; ++i) {
		float theta = i * delta_theta;
		float x = radius * cosf(theta);
		float z = radius * sinf(theta);
		DirectX::XMFLOAT3 normal = { cosf(theta), 0.0f, sinf(theta) };

		vertices.push_back({ {x, -half_height, z}, normal });
		vertices.push_back({ {x, +half_height, z}, normal });
	}

	for (int i = 0; i < slice_count; ++i) {
		indices.push_back(i * 2);
		indices.push_back(i * 2 + 1);
		indices.push_back(i * 2 + 3);

		indices.push_back(i * 2);
		indices.push_back(i * 2 + 3);
		indices.push_back(i * 2 + 2);
	}

	// === 上半球 ===
	int baseIndex = (int)vertices.size();
	for (int stack = 0; stack <= stack_count; ++stack) {
		float phi = DirectX::XM_PIDIV2 * stack / stack_count;  // 0～π/2
		float y = radius * sinf(phi);
		float r = radius * cosf(phi);
		for (int slice = 0; slice <= slice_count; ++slice) {
			float theta = slice * delta_theta;
			float x = r * cosf(theta);
			float z = r * sinf(theta);
			DirectX::XMFLOAT3 pos = { x, y + half_height, z };
			DirectX::XMFLOAT3 norm = { x / radius, y / radius, z / radius };
			vertices.push_back({ pos, norm });
		}
	}

	for (int stack = 0; stack < stack_count; ++stack) {
		for (int slice = 0; slice < slice_count; ++slice) {
			uint32_t i0 = baseIndex + stack * (slice_count + 1) + slice;
			uint32_t i1 = baseIndex + (stack + 1) * (slice_count + 1) + slice;
			uint32_t i2 = baseIndex + (stack + 1) * (slice_count + 1) + slice + 1;
			uint32_t i3 = baseIndex + stack * (slice_count + 1) + slice + 1;

			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}

	// === 下半球 ===
	baseIndex = (int)vertices.size();
	for (int stack = 0; stack <= stack_count; ++stack) {
		float phi = DirectX::XM_PIDIV2 * stack / stack_count;  // 0～π/2
		float y = radius * sinf(phi);
		float r = radius * cosf(phi);
		for (int slice = 0; slice <= slice_count; ++slice) {
			float theta = slice * delta_theta;
			float x = r * cosf(theta);
			float z = r * sinf(theta);
			DirectX::XMFLOAT3 pos = { x, -y - half_height, z };
			DirectX::XMFLOAT3 norm = { x / radius, -y / radius, z / radius };
			vertices.push_back({ pos, norm });
		}
	}

	for (int stack = 0; stack < stack_count; ++stack) {
		for (int slice = 0; slice < slice_count; ++slice) {
			uint32_t i0 = baseIndex + stack * (slice_count + 1) + slice;
			uint32_t i1 = baseIndex + (stack + 1) * (slice_count + 1) + slice;
			uint32_t i2 = baseIndex + (stack + 1) * (slice_count + 1) + slice + 1;
			uint32_t i3 = baseIndex + stack * (slice_count + 1) + slice + 1;

			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i1);

			indices.push_back(i0);
			indices.push_back(i3);
			indices.push_back(i2);
		}
	}

	create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}