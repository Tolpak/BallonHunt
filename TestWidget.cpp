#include "stdafx.h"
#include "TestWidget.h"
#include <fstream>
#include <map>
#include "CannonBall.h"
#include "Target.h"
#include "ScoreManager.h"

const std::string configName = "input.txt";
const int standOffsetY = -15;

TestWidget::TestWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _timer(0)
	, _eff(NULL)
	, _cannonTurn(0.f)
	, _gameOver(false)
	, _angle(0)
	, _ballSpeed(0)
{
	Init();
}

void TestWidget::Init()
{
	ReadConfig();
	scoreManager = std::make_unique<ScoreManager>();

	_balloon = Core::resourceManager.Get<Render::Texture>("balloon");
	_window = Core::resourceManager.Get<Render::Texture>("window");
	_aim = Core::resourceManager.Get<Render::Texture>("aim");
	_background = Core::resourceManager.Get<Render::Texture>("background");
	_stand = Core::resourceManager.Get<Render::Texture>("stand");
	_cannon = Core::resourceManager.Get<Render::Texture>("cannon");
	_cannonBall = Core::resourceManager.Get<Render::Texture>("cannonBall");

	CreateTargets();
}

void TestWidget::ReadConfig()
{
	std::ifstream reader;
	reader.open(configName);

	if (!reader.is_open())
	{
		Log::Info(configName + " not found");
	}
	else
	{
		std::string str;
		std::string delimiter = "=";
		while (!reader.eof())
		{
			str = "";
			std::getline(reader, str);
			std::string identifier = str.substr(0, str.find(delimiter));
			str.erase(0, str.find(delimiter) + delimiter.length());
			config.emplace(identifier, std::stoi(str));
		}
		Log::Info("config is loaded");
		auto it = config.find("Time");
		if (it != config.end())
			_timer = (it->second);

		it = config.find("Speed");
		if (it != config.end())
			_ballSpeed = (it->second);
		
	}
	reader.close();
}

void TestWidget::GameOver()
{
	_effCont.KillAllEffects();
	_cannonTurn = 0;
	projectiles.clear();
	targets.clear();
	_gameOver = true;
}

void TestWidget::NewGame()
{
	auto it = config.find("Time");
	if (it != config.end())
		_timer = (it->second);
	CreateTargets();
	_gameOver = false;
}

void TestWidget::CreateTargets()
{
	auto it = config.find("CountTarget");
	if (it != config.end())
		targets.reserve(it->second);
	else
		targets.reserve(30);

	int maxPoints = 0;
	for (unsigned i = 0; i < targets.capacity() - 1; ++i)
	{
		TargetType type = TargetType::baloon;
		if (rand() % 3 == 0)
			if (rand() % 2 != 0)
				type = TargetType::bomb;
			else
				type = TargetType::clocki;

		targets.push_back(std::make_unique<Target>(type));
		if (type == TargetType::baloon)
			maxPoints++;
	}
	scoreManager->SetMaxScore(maxPoints);
}

void TestWidget::Draw()
{
	IPoint mouse_pos = Core::mainInput.GetMousePos();
	_background->Draw();

	_stand->Draw(FPoint((Render::device.Width() / 2 - _stand->Width() / 2), standOffsetY));
	_effCont.Draw();

	DrawProjectiles();
	DrawCannon(mouse_pos);
	DrawTargets();
	DrawStats();
	DrawAim(mouse_pos);
}

void TestWidget::DrawAim(const IPoint &mouse_pos)
{
	Render::device.PushMatrix();
	Render::device.MatrixTranslate((float)mouse_pos.x, (float)mouse_pos.y, 0);
	IRect texRect1 = _aim->getBitmapRect();
	FRect rect(texRect1);
	FRect uv(0, 1, 0, 1);
	_aim->TranslateUV(rect, uv);
	Render::device.MatrixTranslate(-texRect1.width * 0.5f, -texRect1.height * 0.5f, 0);
	_aim->Bind();
	Render::DrawQuad(rect, uv);
	Render::device.PopMatrix();
}
void TestWidget::DrawCannon(const IPoint &mouse_pos)
{
	Render::device.PushMatrix();
	Render::device.MatrixTranslate(Render::device.Width() / 2, cannonY, 0);

	if (mouse_pos.y > cannonY && !_gameOver)
	{
		float hypotenuse = sqrt(pow(mouse_pos.x - Render::device.Width() / 2, 2) + pow(mouse_pos.y - cannonY, 2));
		_cannonTurn = std::acos((mouse_pos.y - cannonY) / hypotenuse) * 180 / math::PI;
		if (mouse_pos.x > Render::device.Width() / 2)
			_cannonTurn = -_cannonTurn;
	}

	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _cannonTurn);

	IRect texRect1 = _cannon->getBitmapRect();
	FRect rect1(texRect1);
	FRect uv1(0, 1, 0, 1);
	_cannon->TranslateUV(rect1, uv1);
	Render::device.MatrixTranslate(-texRect1.width * 0.5f, -texRect1.height / 3, 0.0f);
	_cannon->Bind();
	Render::DrawQuad(rect1, uv1);
	Render::device.PopMatrix();
}

void TestWidget::DrawProjectiles()
{
	for (unsigned i = projectiles.size(); i > 0; i--)
	{
		FPoint currentPosition = projectiles[i - 1]->GetPosition();
		Render::device.PushMatrix();
		Render::device.MatrixTranslate(currentPosition.x - _cannonBall->Width() / 2, currentPosition.y - _cannonBall->Height() / 2, 0);
		ParticleEffectPtr eff = _effCont.AddEffect("Track");
		eff->posX = projectiles[i - 1]->GetPosition().x;
		eff->posY = projectiles[i - 1]->GetPosition().y;
		eff->Reset();		
		_cannonBall->Draw();

		Render::device.PopMatrix();

		if (currentPosition.y <= cRADIUS / 2)
		{
			projectiles.erase(projectiles.begin() + i - 1);	
		}
		else
		{
			for (unsigned j = targets.size(); j > 0; j--)
			{
				if (projectiles[i - 1]->CheckHit(*(targets[j - 1].get())))
				{
					projectiles.erase(projectiles.begin() + i - 1);
					TargetType type = targets[j - 1]->GetType();

					if (type == TargetType::baloon)
					{
						ParticleEffectPtr eff = _effCont.AddEffect("Balloon");
						eff->posX = targets[j - 1]->GetPosition().x;
						eff->posY = targets[j - 1]->GetPosition().y;
						eff->Reset();
						scoreManager->AddPoint();
					}
					else if (type == TargetType::bomb)
					{
						;
						_timer -= 10;
						ParticleEffectPtr eff = _effCont.AddEffect("Boom");
						eff->posX = targets[j - 1]->GetPosition().x;
						eff->posY = targets[j - 1]->GetPosition().y;
						eff->Reset();
					}
					else if (type == TargetType::clocki)
					{
						_timer += 3;
						ParticleEffectPtr eff = _effCont.AddEffect("Clock");
						eff->posX = targets[j - 1]->GetPosition().x;
						eff->posY = targets[j - 1]->GetPosition().y;
						eff->Reset();
					}
					targets.erase(targets.begin() + j - 1);
					if (scoreManager->IsWin())
					{
						GameOver();
						return;
					}
					break;
				}
			}
		}
	}
}

void TestWidget::DrawTargets()
{
	unsigned size = targets.size();
	for (auto i = 0u; i < size; ++i)
	{
		Target &target = *(targets[i].get());

		FPoint currentPosition = target.GetPosition();

		Render::device.PushMatrix();
		Render::device.MatrixTranslate(currentPosition.x - target.texture->Width() / 2, currentPosition.y - target.texture->Height() / 2, 0);
		target.texture->Draw();
		Render::device.PopMatrix();

		for (auto j = 0u; j < size; ++j)
		{
			if (i != j)
			{
				target.CheckCollision(*(targets[j].get()));
			}
		}
	}
}

void TestWidget::DrawStats()
{
	_window->Draw(Render::device.Width() - _window->Width(), 0);
	Render::BindFont("arial");
	if (_gameOver)
	{
		_balloon->Draw(Render::device.Width() / 2 - _balloon->Width(), Render::device.Height() / 2 - _balloon->Height() / 2);
		Render::BeginColor(Color(255, 0, 0, 255));
		Render::PrintString(Render::device.Width() / 2 + _balloon->Width() / 2, Render::device.Height() / 2 + 10, utils::lexical_cast(scoreManager->GetScore()) + " / " + utils::lexical_cast(scoreManager->GetMaxScore()), 1.3f, CenterAlign);

		Render::PrintString(Render::device.Width() / 2, Render::device.Height() / 3, "PRESS ENTER TO RESTART", 1.2f, CenterAlign);
		Render::EndColor();
	}
	std::string time = "00 : ";
	if (_timer < 10)
		time = "00 : 0";
	Render::BeginColor(Color(255, 0, 0, 255));
	Render::PrintString(Render::device.Width() - _window->Width() / 2, _window->Height() / 2 + 10, time + utils::lexical_cast((int)_timer), 1.5f, CenterAlign);
	Render::EndColor();
}

void TestWidget::Update(float dt)
{
	for (auto i = 0u; i < projectiles.size(); i++)
	{
		projectiles[i]->UpdateTimer(dt);
	}

	for (auto i = 0u; i < targets.size(); i++)
	{
		targets[i]->UpdateTimer(dt);
	}

	_effCont.Update(dt);

	if (_timer > 0 && !_gameOver)
		_timer -= dt;
	else if (!_gameOver)
	{
		GameOver();
	}
	else if (_gameOver && _timer < 0)
		_timer = 0;
}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{
	if (Core::mainInput.GetMouseLeftButton())
	{
		if (!_gameOver)
		{
			projectiles.push_back(std::make_unique<CannonBall>(mouse_pos, _ballSpeed));
		}
	}
	return false;
}

void TestWidget::KeyPressed(int keyCode)
{
	if (_gameOver)
		if (keyCode == VK_RETURN)
			NewGame();
	if (keyCode == VK_ESCAPE)
		Core::appInstance->CloseWindow();
}