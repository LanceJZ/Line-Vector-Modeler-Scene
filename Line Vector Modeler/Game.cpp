#include "Game.h"

Game::Game()
{
	//When adding classes to EM, must be pointer to heap,IE: Name = new Class().

	LogicID = EM.AddCommon(Logic = DBG_NEW GameLogic());
	//BackGroundID = Managers.EM.AddCommon(BackGround = DBG_NEW TheBackground());
	PlayerID = EM.AddLineModel(Player = DBG_NEW ThePlayer());
}

Game::~Game()
{
}

bool Game::Initialize() //Initialize
{
	Common::Initialize();

	//BackGround->Initialize();
	Logic->Initialize();
	Player->Initialize();

	float multi = 1.0f;
	FieldSize = { GetScreenWidth() * multi, (float)GetScreenHeight() };

	Logic->SetPlayer(Player);

	//Any Entities added after this point need this method fired manually.

	return true;
}

bool Game::Load()
{
	Logic->Load();
	return true;
}

bool Game::BeginRun()
{
	//Any Entities added after this point need this method fired manually.

	return true;
}

void Game::ProcessInput()
{
	GameInput();
}


void Game::Update(float deltaTime)
{
	if (Logic->State == Pause)	return;
}

void Game::FixedUpdate(float deltaTime)
{
	if (Logic->State == Pause)	return;
}

void Game::Draw()
{
	BeginMode3D(TheCamera);

	//3D Drawing here.
	Draw3D();

	EndMode3D();

	//2D drawing, fonts go here.

	Draw2D();
}

void Game::Draw3D()
{

#ifdef _DEBUG
	int fsx = int(FieldSize.x * 0.5f);
	int fsy = int(FieldSize.y * 0.5f);

	DrawLine(-fsx, -fsy + 0.5f, fsx, -fsy + 0.5f, { DARKBLUE }); //Top
	DrawLine(fsx, -fsy, fsx, fsy, { DARKBLUE }); //Right
	DrawLine(fsx, fsy - 1, -fsx, fsy, { DARKBLUE }); //Bottom
	DrawLine(-fsx + 0.5f, fsy - 1, -fsx, -fsy, { DARKBLUE }); //Left Side
#endif
}

void Game::Draw2D()
{
}

void Game::GameInput()
{
	Logic->GameInput();
}