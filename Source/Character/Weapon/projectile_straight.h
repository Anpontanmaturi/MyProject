#pragma once

#include "Graphics/skinned_mesh.h"
#include "Character/projectile.h"

// íºê¸íeä€
class ProjectileStraight : public Projectile
{
public:
	ProjectileStraight(ProjectileManager* manager, ID3D11Device* device);
	~ProjectileStraight() override;

	void Update(float elapsed_time) override;

	void Render(ID3D11DeviceContext* device_context) override;

	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	std::unique_ptr<SkinnedMesh> model = nullptr;
	float	speed = 10.0f;
	float	life_timer = 3.0f;
};