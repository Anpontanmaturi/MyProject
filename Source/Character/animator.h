#pragma once

#include "Graphics/skinned_mesh.h"

class Animator
{
public:
	Animator(SkinnedMesh* mesh) : mesh(mesh) {}

	void update(float elapsed_time);

	void play(int clip_index, bool loop, float blend_duration = 0.1f);
	void play(const char* clip_name, bool loop, float blend_duration = 0.1f);

	const Animation::keyframe* get_current_keyframe() const { return current_keyframe; }

private:
	SkinnedMesh*			mesh = nullptr;
	Animation*				current_clip = nullptr;
	Animation::keyframe*	current_keyframe = nullptr;
	Animation::keyframe		blended_keyframe;
	Animation::keyframe		previous_keyframe;
	float					current_time = 0;
	float					duration = 0;
	float					blend_time = 0;
	float					blend_duration = 0;
	bool					is_loop = false;
	bool					is_playing = false;
};
