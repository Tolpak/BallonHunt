#pragma once

#include "MovingObject.h"

class CannonBall : public MovingObject
{
public:
	CannonBall(const IPoint& mouse_pos, int speed);
	CannonBall(const CannonBall&) = delete;
	CannonBall &operator=(const CannonBall&) = delete;

private:
	void Fire(const IPoint& aimPoint);
};
