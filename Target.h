#pragma once

#include "MovingObject.h"

enum TargetType { baloon, bomb, clocki };

const int TargetRadius = 30;

class Target : public MovingObject
{
public:
	Target(TargetType type);
	Target(const Target&) = delete;
	Target &operator=(const Target&) = delete;

	const TargetType &GetType();

private:
	void Init();
private:
	TargetType _type;
};

