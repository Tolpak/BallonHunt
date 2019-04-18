#pragma once
class ScoreManager
{
public:
	ScoreManager();

	int GetScore();
	void AddPoint();
	bool IsWin();
	void SetMaxScore(int points);
	int GetMaxScore();

private:
	int points;
	int maxPoints;
};

