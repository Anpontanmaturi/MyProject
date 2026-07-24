#include <DirectXCollision.h>
#include "collision_manager.h"
#include "collision_cache.h"
#include "Graphics/skinned_mesh.h"

using namespace DirectX;

void CollisionManager::Register(CollisionMesh* mesh)
{
	if (std::filesystem::exists(mesh->cache_filename))
	{
		CollisionCache::Load(collision_mesh, mesh->cache_filename.c_str());
	}
	else
	{
		SpaceDivision(mesh);

		CollisionCache::Save(collision_mesh, mesh->cache_filename.c_str());
	}

	meshes.emplace_back(mesh);
}

void CollisionManager::Unregister(CollisionMesh* mesh)
{
	auto it = std::find(meshes.begin(), meshes.end(), mesh);
	if (it != meshes.end())
	{
		meshes.erase(it);
	}
}

void CollisionManager::SpaceDivision(CollisionMesh* mesh)
{
	collision_mesh.triangles.clear();
	collision_mesh.areas.clear();

	XMVECTOR volume_min = XMVectorReplicate(FLT_MAX);
	XMVECTOR volume_max = XMVectorReplicate(-FLT_MAX);

	for (const SkinnedMesh::mesh& mesh : mesh->mesh->meshes)
	{
		XMMATRIX grobal_transform = XMLoadFloat4x4(&mesh.default_global_transform);
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			// 三角形の頂点を抽出
			uint32_t a = mesh.indices.at(i + 0);
			uint32_t b = mesh.indices.at(i + 1);
			uint32_t c = mesh.indices.at(i + 2);
			XMVECTOR A = XMLoadFloat3(&mesh.vertices.at(a).position);
			XMVECTOR B = XMLoadFloat3(&mesh.vertices.at(b).position);
			XMVECTOR C = XMLoadFloat3(&mesh.vertices.at(c).position);
			A = XMVector3Transform(A, grobal_transform);
			B = XMVector3Transform(B, grobal_transform);
			C = XMVector3Transform(C, grobal_transform);

			// 法線ベクトルを算出
			XMVECTOR N = XMVector3Cross(XMVectorSubtract(B, A), XMVectorSubtract(C, A));
			if (XMVector3Equal(N, XMVectorZero()))
			{
				// 面を作成できない
				continue;
			}
			N = DirectX::XMVector3Normalize(N);

			// 三角形データを格納
			CollisionMesh::Triangle& triangle = collision_mesh.triangles.emplace_back();
			XMStoreFloat3(&triangle.positions[0], A);
			XMStoreFloat3(&triangle.positions[1], B);
			XMStoreFloat3(&triangle.positions[2], C);
			XMStoreFloat3(&triangle.normal, N);

			// モデル全体のAABBを計測
			volume_min = XMVectorMin(volume_min, A);
			volume_min = XMVectorMin(volume_min, B);
			volume_min = XMVectorMin(volume_min, C);
			volume_max = XMVectorMax(volume_max, A);
			volume_max = XMVectorMax(volume_max, B);
			volume_max = XMVectorMax(volume_max, C);
		}
	}

	// モデル全体のAABB
	XMFLOAT3 volumeMin, volumeMax;
	XMStoreFloat3(&volumeMin, volume_min);
	XMStoreFloat3(&volumeMax, volume_max);

	// モデル全体のAABBからXZ平面に指定のサイズで分割されたコリジョンエリアを作成する
	const int cellsize = 4;
	for (float x = volumeMin.x ; x < volumeMax.x ; x += cellsize)
	{
		for (float z = volumeMin.z ; z < volumeMax.z ; z += cellsize)
		{
			// AABBを算出
			CollisionMesh::Area& area = collision_mesh.areas.emplace_back();
			area.bounding_box.Center.x = x + cellsize * 0.5f;
			area.bounding_box.Center.y = (volumeMax.y + volumeMin.y) * 0.5f;
			area.bounding_box.Center.z = z + cellsize * 0.5f;
			area.bounding_box.Extents.x = cellsize * 0.5f;
			area.bounding_box.Extents.y = (volumeMax.y - volumeMin.y) * 0.5f;
			area.bounding_box.Extents.z = cellsize * 0.5f;

			// AABBに所属する三角形を抽出
			int triangle_index = 0;
			for (const CollisionMesh::Triangle& triangle : collision_mesh.triangles)
			{
				XMVECTOR A = XMLoadFloat3(&triangle.positions[0]);
				XMVECTOR B = XMLoadFloat3(&triangle.positions[1]);
				XMVECTOR C = XMLoadFloat3(&triangle.positions[2]);

				if (area.bounding_box.Intersects(A, B, C))
				{
					area.triangle_indices.push_back(triangle_index);
				}
				triangle_index++;
			}
		}
	}
}

bool CollisionManager::Raycast(const XMFLOAT3& start, const XMFLOAT3& end, HitResult& hit_result) const
{
	bool hit = false;
	hit_result.distance = FLT_MAX;
	for (const CollisionMesh* mesh : meshes)
	{
		HitResult  temp_result;
		if (Raycast(mesh, start, end, &temp_result))
		{
			if (temp_result.distance < hit_result.distance)
			{
				hit_result = temp_result;
				hit = true;
			}
		}
	}
	return hit;
}

bool CollisionManager::Raycast(const CollisionMesh* mesh, const XMFLOAT3& start, const XMFLOAT3& end, HitResult* hit_result) const
{
	XMVECTOR RayStart = XMLoadFloat3(&start);
	XMVECTOR RayEnd = XMLoadFloat3(&end);
	XMVECTOR RayVec = RayEnd - RayStart;
	XMVECTOR RayLength = XMVector3Length(RayVec);

	// ワールド空間のレイの長さ
	float distance = XMVectorGetX(RayLength);
	if (distance == 0.0f) return false;

	XMMATRIX W = XMLoadFloat4x4(mesh->transform);
	XMMATRIX C = XMLoadFloat4x4(&mesh->mesh->GetCoordinateSystemTransform());

	bool hit = false;
	for (const SkinnedMesh::mesh& mesh : mesh->mesh->meshes)
	{
		XMMATRIX G = XMLoadFloat4x4(&mesh.default_global_transform);

		// レイをワールド空間からローカル空間へ変換
		XMMATRIX Transform = G * C * W;
		XMMATRIX InverseTransform = XMMatrixInverse(nullptr, Transform);

		XMVECTOR S = XMVector3Transform(RayStart, InverseTransform);
		XMVECTOR E = XMVector3Transform(RayEnd, InverseTransform);
		XMVECTOR SE = E - S;
		XMVECTOR V = XMVector3Normalize(SE);
		XMVECTOR Length = XMVector3Length(SE);

		// レイの長さ
		float length = XMVectorGetX(Length);
		if (length <= 0.0f) continue;

		float neart = length;

		// バウンディングボックスとの交差判定（高速化）
		XMVECTOR BBoxMin = XMLoadFloat3(&mesh.bounding_box[0]);
		XMVECTOR BBoxMax = XMLoadFloat3(&mesh.bounding_box[1]);
		BoundingBox bbox;
		BoundingBox::CreateFromPoints(bbox, BBoxMin, BBoxMax);

		float bbox_distance = length;
		if (!bbox.Intersects(S, V, bbox_distance))
		{
			// バウンディングボックスと交差しない場合はスキップ
			continue;
		}

		// 三角形（面）との交差判定
		const std::vector<SkinnedMesh::vertex>& vertices = mesh.vertices;
		const std::vector<uint32_t>& indices = mesh.indices;

		bool hit_mesh = false;
		XMVECTOR HitPosition;
		XMVECTOR HitNormal;

		if (!space_division)
		{
			for (const SkinnedMesh::mesh::subset& subset : mesh.subsets)
			{
				for (UINT i = 0; i < subset.index_count; i += 3)
				{
					UINT index = subset.start_index_location + i;

					// 三角形の頂点を抽出
					const SkinnedMesh::vertex& a = vertices[indices[index]];
					const SkinnedMesh::vertex& b = vertices[indices[index + 1]];
					const SkinnedMesh::vertex& c = vertices[indices[index + 2]];

					XMVECTOR A = XMLoadFloat3(&a.position);
					XMVECTOR B = XMLoadFloat3(&b.position);
					XMVECTOR C = XMLoadFloat3(&c.position);

					// 三角形の三辺ベクトルを算出
					XMVECTOR AB = B - A;
					XMVECTOR BC = C - B;
					XMVECTOR CA = A - C;

					// 三角形の法線ベクトルを算出		
					XMVECTOR N = XMVector3Cross(AB, BC);

					// 内積の結果がプラスならば裏向き
					XMVECTOR Dot = XMVector3Dot(V, N);
					float dot = XMVectorGetX(Dot);
					if (dot >= 0) continue;

					// 三角形とレイの交差判定
					float dist = neart;
					if (!TriangleTests::Intersects(S, V, A, B, C, dist))
						continue;

					if (dist >= neart) continue;

					neart = dist;
					HitPosition = S + V * neart;
					HitNormal = N;

					hit_mesh = true;
				}
			}
		}
		else
		{
			for (const CollisionMesh::Area& area : this->collision_mesh.areas)
			{
				float area_dist = neart;
				if (!area.bounding_box.Intersects(S, V, area_dist))continue;

				for (int index : area.triangle_indices)
				{
					const CollisionMesh::Triangle triangle = this->collision_mesh.triangles[index];

					XMVECTOR A = XMLoadFloat3(&triangle.positions[0]);
					XMVECTOR B = XMLoadFloat3(&triangle.positions[1]);
					XMVECTOR C = XMLoadFloat3(&triangle.positions[2]);

					XMVECTOR N = XMLoadFloat3(&triangle.normal);

					XMVECTOR Dot = XMVector3Dot(V, N);
					float dot = XMVectorGetX(Dot);
					if (dot >= 0) continue;

					// 三角形とレイの交差判定
					float dist = neart;
					if (!TriangleTests::Intersects(S, V, A, B, C, dist))
						continue;

					if (dist >= neart) continue;

					neart = dist;
					HitPosition = S + V * neart;
					HitNormal = N;

					hit_mesh = true;
				}
			}
		}
		if (hit_mesh)
		{
			// ローカル空間からワールド空間へ変換
			XMVECTOR WorldHitPosition = XMVector3Transform(HitPosition, Transform);
			XMVECTOR WorldHitVec = WorldHitPosition - RayStart;
			XMVECTOR WorldHitDistance = XMVector3Length(WorldHitVec);
			float world_hit_distance = XMVectorGetX(WorldHitDistance);

			// ヒット情報保存
			if (distance > world_hit_distance)
			{
				distance = world_hit_distance;
				if (hit_result != nullptr)
				{
					hit_result->distance = world_hit_distance;
					XMStoreFloat3(&hit_result->position, WorldHitPosition);
					XMStoreFloat3(&hit_result->normal, XMVector3Normalize(XMVector3TransformNormal(HitNormal, Transform)));
				}
				hit = true;
			}
		}
	}

	return hit;
}

// 球と球の交差判定
bool CollisionManager::SphereVsSphere(
	const XMFLOAT3& positionA,
	float radiusA,
	const XMFLOAT3& positionB,
	float radiusB,
	XMFLOAT3& outPositionB)
{
	// B→Aの単位ベクトルを算出
	XMVECTOR PositionA = XMLoadFloat3(&positionA);
	XMVECTOR PositionB = XMLoadFloat3(&positionB);
	XMVECTOR Vec = XMVectorSubtract(PositionB, PositionA);
	XMVECTOR LengthSq = XMVector3LengthSq(Vec);
	float lengthSq;
	XMStoreFloat(&lengthSq, LengthSq);

	// 距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}

	// AがBを押し出す
	Vec = XMVector3Normalize(Vec);
	Vec = XMVectorScale(Vec, range);
	PositionB = XMVectorAdd(PositionA, Vec);
	XMStoreFloat3(&outPositionB, PositionB);

	return true;
}


// 円柱と円柱の交差判定
bool CollisionManager::CylinderVsCylinder(
	const XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	XMFLOAT3& outPositionB)
{
	// Aの足元がBの頭より上なら当たっていない
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}
	// Aの頭がBの足元より下なら当たっていない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}
	// XZ平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}
	// AがBを押し出す
	vx /= distXZ;
	vz /= distXZ;
	outPositionB.x = positionA.x + (vx * range);
	outPositionB.y = positionB.y;
	outPositionB.z = positionA.z + (vz * range);

	return true;
}

// 球と円柱の交差判定
bool CollisionManager::SphereVsCylinder(
	const XMFLOAT3& spherePosition,
	float sphereRadius,
	const XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight,
	XMFLOAT3& outCylinderPosition)
{
	// 高さチェック
	if (spherePosition.y + sphereRadius < cylinderPosition.y) return false;
	if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight) return false;

	// XZ平面での範囲チェック
	float vx = cylinderPosition.x - spherePosition.x;
	float vz = cylinderPosition.z - spherePosition.z;
	float range = sphereRadius + cylinderRadius;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range) return false;

	// 球が円柱を押し出す
	vx /= distXZ;
	vz /= distXZ;
	outCylinderPosition.x = spherePosition.x + (vx * range);
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = spherePosition.z + (vz * range);

	return true;
}