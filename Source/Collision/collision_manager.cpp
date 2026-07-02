#include <DirectXCollision.h>
#include "collision_manager.h"
#include "Graphics/skinned_mesh.h"

using namespace DirectX;

void CollisionManager::Register(CollisionMesh* mesh)
{
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

bool CollisionManager::Raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit_result) const
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

bool CollisionManager::Raycast(const CollisionMesh* mesh, const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult* hit_result) const
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
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB)
{
	// B→Aの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// 距離判定
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}

	// AがBを押し出す
	Vec = DirectX::XMVector3Normalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	PositionB = DirectX::XMVectorAdd(PositionA, Vec);
	DirectX::XMStoreFloat3(&outPositionB, PositionB);

	return true;
}


// 円柱と円柱の交差判定
bool CollisionManager::CylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outPositionB)
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
	const DirectX::XMFLOAT3& spherePosition,
	float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition)
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