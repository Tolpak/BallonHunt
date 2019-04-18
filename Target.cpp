#include "stdafx.h"
#include "Target.h"

const int respawnOffset = 100;
const int minSpeed = 100;
const int maxSpeed = 150;
const char* targetTypes[] = { "balloon","bomb","clocki"};

Target::Target(TargetType targetType)
	:_type(targetType)
{
	Init();
}

void Target::Init()
{
	texture = Core::resourceManager.Get<Render::Texture>(targetTypes[_type]);

	int startX = respawnOffset / 2 + rand() % (Render::device.Width() - respawnOffset);
	int startY = respawnOffset * 2 + rand() % (Render::device.Height() - respawnOffset * 2 - respawnOffset / 2 );
	int y, x;
	int dir = rand() % 4;
	if (dir < 2)
	{
		y = rand() % (Render::device.Height() );
		if (dir == 0)
			x = 0;
		else
			x = Render::device.Width();
	}
	else
	{
		x = rand() % (Render::device.Width() );
		if (dir == 3)
			y = 0;
		else
			y = Render::device.Height();
	}
	float speed = minSpeed + rand() % (maxSpeed - minSpeed);
	SetSpeed(speed);
	float dist = sqrt(pow(x - startX, 2) + pow(y - startY, 2));
	CalculateTimeDivider(dist);
	CreateSpline(IPoint(startX, startY),IPoint(x, y));
}

const TargetType &Target::GetType()
{
	const TargetType &rType = _type;
	return rType;
}
