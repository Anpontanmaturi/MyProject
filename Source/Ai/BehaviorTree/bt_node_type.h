#pragma once

enum class BTNodeType
{
	// ƒm[ƒh’Ç‰Á•ÏX‚·‚é
	Root = 0,

	Sequence, // ã‚©‚ç‡‚É
	Selector, // ’T‚·

	Wait,		// s“®Œn
	MoveTo,
	MoveStop,
	Idle,
	ShotStreight,

	Escape = 50,

	DistCondition, // ğŒŒn
	CheckBool,

	Inverter = 70, // ”½“]
	Repeater, // ŒJ‚è•Ô‚µ
};