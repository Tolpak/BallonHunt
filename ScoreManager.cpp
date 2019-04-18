#include "stdafx.h"
#include "ScoreManager.h"


ScoreManager::ScoreManager()
	: points(0)
	, maxPoints(0)
{
}

void ScoreManager::AddPoint()
{
	points++;
}

void ScoreManager::SetMaxScore(int max)
{
	maxPoints = max;
	points = 0;
}

int ScoreManager::GetScore()
{
	return points;
}

int ScoreManager::GetMaxScore()
{
	return maxPoints;
}

bool ScoreManager::IsWin()
{
	if (points == maxPoints)
		return true;
	else
		return false;
}


