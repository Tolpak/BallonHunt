#include "stdafx.h"
#include "MovingObject.h"

MovingObject::MovingObject()
	: _timeDivider(1)
	, _timer(0)
{
}

const int FindDistance(MovingObject &object1, MovingObject &object2)
{
	int x1 = object1.GetPosition().x;
	int x2 = object2.GetPosition().x;
	int y1 = object1.GetPosition().y;
	int y2 = object2.GetPosition().y;

	int x = abs(x1 - x2);
	int y = abs(y1 - y2);

	int min = x > y ? y : x;
	return(x + y - (min >> 1) - (min >> 2) + (min >> 4));
}

FPoint MovingObject::GetPosition()
{
	return spline.getGlobalFrame(math::clamp(0.0f, 1.0f, _timer));
	
}
void MovingObject::SetSpeed(float speed)
{
	_speed = speed;
}

void MovingObject::CalculateTimeDivider(float path)
{
	_timeDivider = path / _speed;
}

float MovingObject::FindAngle(FPoint point)
{

	float newX, newY, hypo;
	newX = abs(spline.GetKey(0).x - point.x);
	newY = abs(point.y - spline.GetKey(0).y);
	hypo = sqrt(pow(newX, 2) + pow(newY, 2));
	float angle = std::acos(newX / hypo) * 180 / math::PI;
	if (!math::IsFloatNaN(angle))
		return angle;
	else
		return 0;
}

void MovingObject::UpdateTimer(float dt)
{
	_timer += dt / _timeDivider;
	CheckScreenCollision();
}

void MovingObject::CreateSpline(FPoint start, FPoint nextDestination)
{
	_timer = 0;
	spline.Clear();
	spline.addKey(0.0f, start);
	spline.addKey(1.0f, nextDestination);
	spline.CalculateGradient();
}

bool MovingObject::CheckHit(MovingObject& object)
{
	const int dist = FindDistance(*this, object);
	if (dist <= 2 * cRADIUS)
		return true;
	else 
		return false;
}
void MovingObject::CheckCollision(MovingObject& object)
{

	const int dist = FindDistance(*this, object);
	if (dist <= 2 * cRADIUS)
	{
		if (_isNotCollidable)
			return;
		_isNotCollidable = true;
		bool topY = false;
		bool rightX = false;
		float t = cRADIUS / _speed + _timer;
		FPoint vectorXY = spline.getGlobalFrame(math::clamp(0.0f, 1.0f, t));
		float x1 = GetPosition().x; 
		float x2 = object.GetPosition().x;
		float y1 = GetPosition().y;
		float y2 = object.GetPosition().y;
		FPoint newV = FPoint((x1 - x2) / 2, (y1 - y2) / 2);
		FPoint dest;

		dest = FPoint(vectorXY.x + newV.x, vectorXY.y + newV.y);
		float fx = abs(y1 - dest.y) / abs(x1 - dest.x);
		float x = x1, y = y1;
		float newX, newY, hypo;
		if (x1 < dest.x)
		{
			rightX = true;
			x = Render::device.Width() - x;
		}
		if (y1 < dest.y)
		{
			topY = true;
			y = Render::device.Height() - y;
		}
		newY = fx * x;
		if (newY > y)
		{
			newX = y / fx;
			hypo = sqrt(pow(y, 2) + pow(newX, 2));
			if (topY)
				newY = Render::device.Height();
			else
				newY = 0;
			if (rightX)
				newX = newX + x1;
			else
				newX = x1 - newX;
		}
		else {
			hypo = sqrt(pow(newY, 2) + pow(x, 2));

			if (rightX)
				newX = Render::device.Width();
			else
				newX = 0;
			if (topY)
				newY = newY + y1;
			else
				newY = y1 - newY;
		}
		CalculateTimeDivider(hypo);
		CreateSpline(FPoint(x1, y1), FPoint(newX, newY));
	}
	_isNotCollidable = false;
}

void MovingObject::CheckScreenCollision()
{
	FPoint currentPosition = GetPosition();

	if (currentPosition.x + cRADIUS > Render::device.Width() || currentPosition.x - cRADIUS < 0)
	{
		if (_isNotCollidable)
			return;
		_isNotCollidable = true;
		float newY, newX, x, y, oldX;
		float angle = FindAngle(currentPosition);

		if (currentPosition.x - cRADIUS < 0)
		{
			x = Render::device.Width() - currentPosition.x;
			oldX = cRADIUS;
		}
		else
		{
			x = currentPosition.x;
			oldX = Render::device.Width() - cRADIUS;
		}
		if (spline.GetKey(1).y > spline.GetKey(0).y)
			y = Render::device.Height() - currentPosition.y;
		else
			y = currentPosition.y;

		float hypo = x / cos(angle * math::PI / 180.0);
		newY = sqrt(pow(hypo, 2) - pow(x, 2));

		if (newY + y > Render::device.Height())
		{
			hypo = y / sin(angle * math::PI / 180);
			newX = sqrt(pow(hypo, 2) - pow(y, 2));
			newY = Render::device.Height();
			if (spline.GetKey(0).y > spline.GetKey(1).y)
				newY = 0;
			if (currentPosition.x + cRADIUS > Render::device.Width())
				newX = Render::device.Width() - newX;
		}
		else
		{
			newX = 0;
			if (spline.GetKey(0).y > spline.GetKey(1).y)
				newY = currentPosition.y - newY;
			else
				newY += currentPosition.y;
			if (currentPosition.x - cRADIUS < 0)
				newX = Render::device.Width();
		}
		CalculateTimeDivider(hypo);
		CreateSpline(FPoint(oldX, currentPosition.y), FPoint(newX, newY));
	}
	else if (currentPosition.y + cRADIUS > Render::device.Height() || currentPosition.y - cRADIUS < 0)
	{
		if (_isNotCollidable)
			return;
		_isNotCollidable = true;
		float newY, newX, x, y, oldY;
		float angle = 90 - FindAngle(currentPosition);

		if (currentPosition.y - cRADIUS < 0)
		{
			y = Render::device.Height() - currentPosition.y;
			oldY = cRADIUS;
		}
		else
		{
			y = currentPosition.y;
			oldY = Render::device.Height() - cRADIUS;
		}
		if (spline.GetKey(1).x > spline.GetKey(0).x)
			x = Render::device.Width() - currentPosition.x;
		else
			x = currentPosition.x;

		float hypo = y / cos(angle * math::PI / 180.0);
		newX = sqrt(pow(hypo, 2) - pow(y, 2));

		if (newX > x)
		{
			hypo = x / sin(angle * math::PI / 180);
			newY = sqrt(pow(hypo, 2) - pow(x, 2));
			newX = Render::device.Width();
			if (spline.GetKey(0).x > spline.GetKey(1).x)
				newX = 0;
			if (currentPosition.y + cRADIUS > Render::device.Height())
				newY = Render::device.Height() - newY;
		}
		else
		{
			newY = 0;
			if (spline.GetKey(0).x < spline.GetKey(1).x)
				newX = currentPosition.x + newX;
			else
				newX = currentPosition.x - newX;
			if (currentPosition.y - cRADIUS < 0)
				newY = Render::device.Height();
		}

		CalculateTimeDivider(hypo);
		CreateSpline(FPoint(currentPosition.x, oldY), FPoint(newX, newY));
	}
	_isNotCollidable = false;
}