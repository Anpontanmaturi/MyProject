#pragma once

#include <vector>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <fstream>
#include <filesystem>

class SkinnedMesh;

struct HitResult
{
	DirectX::XMFLOAT3	position;
	DirectX::XMFLOAT3	normal;
	float				distance;
};

struct CollisionMesh
{
	struct Triangle
	{
		DirectX::XMFLOAT3	positions[3];
		DirectX::XMFLOAT3	normal;
	};
	struct Area
	{
		DirectX::BoundingBox	bounding_box;
		std::vector<int>		triangle_indices;
	};

	std::vector<Triangle>	triangles;
	std::vector<Area>		areas;

	const SkinnedMesh* mesh = nullptr;
	const DirectX::XMFLOAT4X4* transform = nullptr;

	std::string cache_filename;
};

class CollisionManager
{
private:
	CollisionManager() = default;
	~CollisionManager() = default;
public:
	static CollisionManager& Instance()
	{
		static CollisionManager instance;
		return instance;
	}

public:
	void Register(CollisionMesh* mesh);
	void Unregister(CollisionMesh* mesh);

	bool Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit_result) const;

	// ‹…vs‹…
	static bool SphereVsSphere(const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outPositionB);

	// ‰~’Œvs‰~’Œ
	static bool CylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	// ‹…vs‰~’Œ
	static bool SphereVsCylinder(
		const DirectX::XMFLOAT3& spherePosition,
		float sphereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition
	);

	void SetDivision(const bool space_division) { this->space_division = space_division; }

private:
	bool Raycast(const CollisionMesh* mesh, const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult* hit_result) const;

	// ‹óŠÔ•ªŠ„ƒŒƒCƒLƒƒƒXƒg—p
	void SpaceDivision(CollisionMesh* mesh);

private:
	std::vector<CollisionMesh*> meshes;
	CollisionMesh collision_mesh;
	bool space_division = true;
};