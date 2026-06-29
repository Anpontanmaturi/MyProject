#pragma once

#include <vector>
#include "projectile.h"
#include <d3d11.h>

// ’eŠÛƒ}ƒl[ƒWƒƒ[
class ProjectileManager
{
public:
	ProjectileManager();
	~ProjectileManager();

	static ProjectileManager& Instance();

	void Update(float elapsed_time);

	void Render(ID3D11DeviceContext* device_context);

	void Register(Projectile* projectile); // ’eŠÛ“o˜^

	void Clear(); // ‘Síœ

	int GetProjectileCount() { return static_cast<int>(projectiles.size()); } // ’eŠÛ”æ“¾

	Projectile* GetProjectile(int index) { return projectiles.at(index); } // ’eŠÛæ“¾

	void Remove(Projectile* projectile); // ’eŠÛíœ

private:
	ProjectileManager(const ProjectileManager&) = delete;
	ProjectileManager& operator=(const ProjectileManager&) = delete;

	std::vector<Projectile*>		projectiles;
	std::vector<Projectile*>		removes;
};