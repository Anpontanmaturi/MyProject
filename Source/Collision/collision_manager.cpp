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
	XMMATRIX C = XMLoadFloat4x4(&mesh->mesh->get_coordinate_system_transform());

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
