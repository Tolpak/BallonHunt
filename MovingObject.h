#pragma once

#include "stdafx.h"

const int cRADIUS = 30;
const int cannonY = 55;

class MovingObject
{
public:
	MovingObject();
	bool CheckHit(MovingObject& object);
	void CreateSpline(FPoint start, FPoint nextDestination);
	void UpdateTimer(float dt);
	void CalculateTimeDivider(float path);
	void SetSpeed(float speed);
	void CheckCollision(MovingObject &object);
	FPoint GetPosition();

public:
	Render::Texture* texture;

private:
	float FindAngle(FPoint point);
	void CheckScreenCollision();

private:
	float _timeDivider;
	float _timer;
	float _speed;
	bool _isNotCollidable;
	TimedSpline<FPoint> spline;
};

