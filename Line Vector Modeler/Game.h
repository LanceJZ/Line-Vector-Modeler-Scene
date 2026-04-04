#pragma once
#include <memory>
#include "Globals.h"
#include "GameLogic.h"
#include "ThePlayer.h"
#include "TheBackground.h"

class Game : Common
{
public:
	Game();
	virtual ~Game();

	GameLogic* Logic = {};

	bool Initialize();
	bool Load();
	bool BeginRun();
	void ProcessInput();
	void Update(float deltaTime);
	void FixedUpdate(float deltaTime);
	void Draw();
	void Draw3D();
	void Draw2D();

private:
	void GameInput();

	size_t LogicID = 0;
	size_t BackGroundID = 0;
	size_t PlayerID = 0;
	size_t EnemiesID = 0;

	//When adding classes to EM, must be heap pointer.
	//TheBackground* BackGround = {};
	ThePlayer* Player = {};
};