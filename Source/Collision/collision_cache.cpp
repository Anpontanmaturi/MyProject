#include "collision_cache.h"

bool CollisionCache::Save(const CollisionMesh& data, const char* filename)
{
	std::ofstream ofs(filename, std::ios::binary);

	if (!ofs)return false;

	// 三角形の数を保存
	size_t triangle_count = data.triangles.size();
	ofs.write(reinterpret_cast<const char*>(&triangle_count), sizeof(size_t));

	// 三角形を保存
	ofs.write(reinterpret_cast<const char*>(data.triangles.data()),
		sizeof(CollisionMesh::Triangle) * triangle_count);

	// エリアの数を保存
	size_t area_count = data.areas.size();
	ofs.write(reinterpret_cast<const char*>(&area_count), sizeof(size_t));

	for (auto& area : data.areas)
	{
		ofs.write(reinterpret_cast<const char*>(&area.bounding_box), sizeof(DirectX::BoundingBox));

		size_t index_count = area.triangle_indices.size();
		ofs.write(reinterpret_cast<const char*>(&index_count), sizeof(size_t));

		ofs.write(reinterpret_cast<const char*>(area.triangle_indices.data()), sizeof(int) * index_count);
	}
	return ofs.good();
}

bool CollisionCache::Load(CollisionMesh& data, const char* filename)
{
	std::ifstream ifs(filename, std::ios::binary);

	if (!ifs)return false;

	data.triangles.clear();
	data.areas.clear();

	size_t triangle_count;
	ifs.read(reinterpret_cast<char*>(&triangle_count), sizeof(size_t));

	data.triangles.resize(triangle_count);

	ifs.read(reinterpret_cast<char*>(data.triangles.data()),
		sizeof(CollisionMesh::Triangle) * triangle_count);

	size_t area_count;

	ifs.read(reinterpret_cast<char*>(&area_count), sizeof(size_t));

	data.areas.resize(area_count);

	for (auto& area : data.areas)
	{
		ifs.read(reinterpret_cast<char*>(&area.bounding_box), sizeof(DirectX::BoundingBox));

		size_t index_count;

		ifs.read(reinterpret_cast<char*>(&index_count), sizeof(size_t));

		area.triangle_indices.resize(index_count);
		ifs.read(reinterpret_cast<char*>(area.triangle_indices.data()),
			sizeof(int) * index_count);
	}
	return ifs.good();
}

std::string CollisionCache::CreateCachePath(
	const std::string& model_path)
{
	namespace fs = std::filesystem;

	fs::path path(model_path);

	path.replace_extension(".col");

	return path.string();
}