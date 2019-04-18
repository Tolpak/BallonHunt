#include "stdafx.h"
#include "CannonBall.h"

const int cannonBallSpeed = 50;
CannonBall::CannonBall(const IPoint& mouse_pos, int speed)
{
	SetSpeed(speed);
	Fire(mouse_pos);
}

void CannonBall::Fire(const IPoint& aimPoint)
{
	float y = aimPoint.y - cannonY;
	float x = aimPoint.x;
	bool rightDir = false;

	if (x > Render::device.Width() / 2)
	{
		x -= Render::device.Width() / 2;
		rightDir = true;
	}
	else
	{
		x = Render::device.Width() / 2 - x;
	}
	float hypo;
	float a = y / x;
	if (Render::device.Height() < a * Render::device.Width() / 2)
	{
		y = Render::device.Height() - cannonY;
		x = y / a;
		hypo = sqrt(pow(y, 2) + pow(x, 2));
		y = Render::device.Height();
		if (rightDir)
			x += Render::device.Width() / 2;
		else
			x = Render::device.Width() / 2 - x;

	}
	else
	{
		x = Render::device.Width() / 2;
		y = x * a;
		hypo = sqrt(pow(y, 2) + pow(x, 2));
		y += cannonY;
		if (rightDir)
			x = Render::device.Width();
		else
			x = 0;
	}
	CalculateTimeDivider(hypo);
	CreateSpline(FPoint((float)Render::device.Width() / 2, cannonY), FPoint(x, y));
}