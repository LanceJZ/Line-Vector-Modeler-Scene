#include "GameLogic.h"

GameLogic::GameLogic()
{
	EM.AddCommon(Main = new TheMainMenu());
}

GameLogic::~GameLogic()
{
}

void GameLogic::SetPlayer(ThePlayer* player)
{
	Player = player;
	Main->SetPlayer(player);
}

bool GameLogic::Initialize()
{
	Common::Initialize();

	Main->Initialize();

	AdjustedFieldSize = Vector2Multiply(FieldSize, { 0.5f, 0.5f });

	State = MainMenu;

	return true;
}

bool GameLogic::Load()
{
	Main->Load();

	return false;
}

bool GameLogic::BeginRun()
{
	Common::BeginRun();

	Main->BeginRun();

	GameEnded = true;

	return false;
}

void GameLogic::FixedUpdate()
{
	Common::FixedUpdate();

	switch (State)
	{
	case PlayerHit:
		ThePlayerHit();
		break;
	case Ended:
		EndTheGame();
		break;
	case InPlay:
		GameInPlay();
		break;
	case Pause:
		GamePaused();
		break;
	case HighScores:
		DisplayHighScores();
		break;
	case MainMenu:
		GameMainMenu();
		break;
	default:
		break;
	}
}

void GameLogic::GameInput()
{
}

void GameLogic::ThePlayerHit()
{
}

void GameLogic::EndTheGame()
{
}

void GameLogic::NewGame()
{
	Player->NewGame();
}

void GameLogic::GameInPlay()
{
}

void GameLogic::GamePaused()
{
	if (IsKeyPressed(KEY_P) || (IsGamepadAvailable(0)
		&& IsGamepadButtonPressed(0, 13)))
	{
		State = InPlay;
	}
}

void GameLogic::DisplayHighScores()
{
}

void GameLogic::GameMainMenu()
{
	if (!GameEnded)
	{
	}
	else
	{
		if (IsGamepadAvailable(0))
		{
			if (IsGamepadButtonPressed(0, 15))//Start button
			{
			}
		}
		else if (IsKeyPressed(KEY_N))
		{
		}
	}
}