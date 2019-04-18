#pragma once

class CannonBall;
class Target;
class ScoreManager;
///
/// Виджет - основной визуальный элемент на экране.
/// Он отрисовывает себя, а также может содержать другие виджеты.
///
class TestWidget : public GUI::Widget
{
public:
	TestWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;
	bool MouseDown(const IPoint& mouse_pos) override;
	void KeyPressed(int keyCode) override;

private:
	void Init();
	void ReadConfig();
	void CreateTargets();
	void NewGame();
	void GameOver();

	void DrawCannon(const IPoint& mouse_pos);
	void DrawAim(const IPoint& mouse_pos);
	void DrawProjectiles();
	void DrawTargets();
	void DrawStats();

private:
	float _timer;
	float _cannonTurn;
	float _angle;
	int _ballSpeed;
	bool _gameOver;

	std::vector<std::unique_ptr<CannonBall>> projectiles;
	std::vector<std::unique_ptr<Target>> targets;
	std::unique_ptr<ScoreManager> scoreManager;
	std::map<std::string, int> config;

	Render::Texture* _balloon;
	Render::Texture* _window;
	Render::Texture* _stand;
	Render::Texture* _background;
	Render::Texture* _cannon;
	Render::Texture* _aim;
	Render::Texture* _cannonBall;

	EffectsContainer _effCont;
	ParticleEffectPtr _eff;
	TimedSpline<FPoint> spline;
};
