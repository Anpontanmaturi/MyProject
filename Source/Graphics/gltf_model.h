#pragma once
#define NOMINMAX
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "../tinygltf-release/tiny_gltf.h"

class GltfModel
{
	std::string filename;
public:
	GltfModel(ID3D11Device* device, const std::string& filename);
	virtual ~GltfModel() = default;
	struct scene
	{
		std::string name;
		std::vector<int> nodes; // Array of 'root' nodes
	};
	std::vector<scene> scenes;
	int default_scene = 0;

	struct node
	{
		std::string name;
		int skin{ -1 }; // index of skin referenced by this node
		int mesh{ -1 }; // index of mesh referenced by this node

		std::vector<int> children; // An array of indices of child nodes of this node

		// Local transforms
		DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
		DirectX::XMFLOAT3 scale{ 1, 1, 1 };
		DirectX::XMFLOAT3 translation{ 0, 0, 0 };

		DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	};
	std::vector<node> nodes;

	void FetchNodes(const tinygltf::Model& gltf_model);
	void CumulateTransforms(std::vector<node>& nodes);

	struct buffer_view
	{
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		int buffer{ -1 };
		size_t stride_in_bytes{ 0 };
		size_t byte_offset{ 0 };
		size_t count{ 0 };
	};
	struct mesh
	{
		std::string name;
		struct primitive
		{
			int material;
			std::map<std::string, buffer_view> vertex_buffer_views;
			buffer_view index_buffer_view;

			bool has(const char* attribute) const
			{
				return vertex_buffer_views.find(attribute) != vertex_buffer_views.end()
					&& vertex_buffer_views.at(attribute).buffer > -1;
			}
		};
		std::vector<primitive> primitives;
	};
	std::vector<mesh> meshes;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> buffers;

	void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	struct primitive_constants
	{
		DirectX::XMFLOAT4X4 world;
		int material{ -1 };
		int has_tangent{ 0 };
		int skin{ -1 };
		int pad;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_cbuffer;

	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world);

	struct texture_info
	{
		int index = -1;
		int texcoord = 0;
	};
	struct normal_texture_info
	{
		int index = -1;
		int texcoord = 0;
		float scale = 1;
	};
	struct occlusion_texture_info
	{
		int index = -1;
		int texcoord = 0;
		float strength = 1;
	};
	struct pbr_metallic_roughness
	{
		float basecolor_factor[4] = { 1, 1, 1, 1 };
		texture_info basecolor_texture;
		float metallic_factor = 1;
		float roughness_factor = 1;
		texture_info metallic_roughness_texture;
	};
	struct material {
		std::string name;
		struct cbuffer
		{
			float emissive_factor[3] = { 0, 0, 0 };
			int alpha_mode = 0; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
			float alpha_cutoff = 0.5f;
			int double_sided = 0;

			pbr_metallic_roughness pbr_metallic_roughness;

			normal_texture_info normal_texture;
			occlusion_texture_info occlusion_texture;
			texture_info emissive_texture;
		};
		cbuffer data;

	};
	std::vector<material> materials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;

	void FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltf_model);
};