#include "projectile_manager.h"

static ProjectileManager* instance = nullptr;

ProjectileManager& ProjectileManager::Instance()
{
	static ProjectileManager instance;
	return instance;
}

// ƒRƒ“ƒXƒgƒ‰ƒNƒ^
ProjectileManager::ProjectileManager()
{
}

// ƒfƒXƒgƒ‰ƒNƒ^
ProjectileManager::~ProjectileManager()
{
	Clear();
}

void ProjectileManager::Update(float elapsed_time)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Update(elapsed_time);
	}

	std::vector<Projectile*> pending;
	pending.swap(removes);

	for (Projectile* projectile : pending)
	{
		auto it = std::find(
			projectiles.begin(),
			projectiles.end(),
			projectile
		);

		// “o˜^‚µ‚Ä‚éê‡íœ
		if (it != projectiles.end())
		{
			projectiles.erase(it);
			delete projectile;
		}
	}
	removes.clear();
}

void ProjectileManager::Render(ID3D11DeviceContext* device_context)
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render(device_context);
	}
}

void ProjectileManager::DrawDebugPrimitive()
{
	for (Projectile* projectile : projectiles)
	{
		projectile->DrawDebugPrimitive();
	}
}

// ’eŠÛ“o˜^
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

// ‘Síœ
void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

// ’eŠÛíœ
void ProjectileManager::Remove(Projectile* projectile)
{
	if (!projectile)
		return;

	// “o˜^‚³‚ê‚Ä‚¢‚È‚¢’eŠÛ‚Í–³‹
	auto it = std::find(
		projectiles.begin(),
		projectiles.end(),
		projectile
	);

	if (it == projectiles.end())
		return;

	// ‚·‚Å‚Éíœ—\–ñ‚³‚ê‚Ä‚¢‚éê‡‚Í–³‹
	auto remove_it = std::find(
		removes.begin(),
		removes.end(),
		projectile
	);

	if (remove_it != removes.end())
		return;


	removes.emplace_back(projectile);
}