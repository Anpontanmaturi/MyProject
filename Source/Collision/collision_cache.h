#pragma once

#include <filesystem>
#include "collision_manager.h"

class CollisionCache
{
public:
	// ファイルの保存、読み込み
	static bool Save(const CollisionMesh& data, const char* filename);
	static bool Load(CollisionMesh& data, const char* filename);

	// パス変更関数
	static std::string CreateCachePath(
		const std::string& model_path
	);
};
