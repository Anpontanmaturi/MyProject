#include "animator.h"

void Animator::update(float elapsed_time)
{
	if (!is_playing) return;

	// 時間進行
	current_time += elapsed_time;
	if (current_time >= duration)
	{
		if (is_loop)
		{
			current_time -= duration;
		}
		else
		{
			current_time = duration;
			is_playing = false;
		}
	}

	// 現在のポーズを取得
	float frame = current_time * current_clip->sampling_rate;
	float factor = std::modf(frame, &frame);
	int frame_index = static_cast<int>(frame);
	int sequence_size = static_cast<int>(current_clip->sequence.size());
	if (!is_loop && frame_index >= sequence_size - 1)
	{
		current_keyframe = &current_clip->sequence.back();
	}
	else
	{
		frame_index = frame_index % sequence_size;
		if (factor == 0)
		{
			current_keyframe = &current_clip->sequence.at(frame_index);
		}
		else
		{
			const Animation::keyframe* keyframes[2] =
			{
				&current_clip->sequence.at(frame_index),
				&current_clip->sequence.at((frame_index + 1) % current_clip->sequence.size())
			};
			mesh->blend_animations(keyframes, factor, blended_keyframe);
			current_keyframe = &blended_keyframe;
		}
	}

	// 過去と現在のポーズをブレンディング
	float blend_factor = 1.0f;
	if (blend_time < blend_duration)
	{
		blend_time += elapsed_time;
		if (blend_time >= blend_duration)
		{

		}
	}
}