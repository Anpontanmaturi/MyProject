#pragma once

enum class BTNodeType
{
	Root = 0,

	Sequence, // ã‚©‚ç‡‚É
	Selector, // ’T‚·

	Wait,
	MoveTo,
	Idle,
	DistCondition,

	Inverter, // ”½“]
	Repeater, // ŒJ‚è•Ô‚µ
};