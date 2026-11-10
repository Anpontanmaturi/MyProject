#pragma once

enum class BTNodeType
{
	// ƒm[ƒh’Ç‰Á•ÏX‚·‚é
	Root = 0,

	Sequence, // ã‚©‚ç‡‚É
	Selector, // ’T‚·

	Wait,		// s“®Œn
	MoveTo,
	Idle,
	ShotStreight,

	DistCondition, // ğŒŒn

	Inverter, // ”½“]
	Repeater, // ŒJ‚è•Ô‚µ
};