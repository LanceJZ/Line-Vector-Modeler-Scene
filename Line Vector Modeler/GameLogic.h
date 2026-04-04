#pragma once
#include "Globals.h"
#include "ThePlayer.h"
#include "TheMainMenu.h"

enum GameState
{
	PlayerHit,
	Ended,
	InPlay,
	Pause,
	HighScores,
	MainMenu
};

class GameLogic : public Common
{
public:
	GameLogic();
	virtual ~GameLogic();

	bool PlayBackgroundMusic = true;
	GameState State = MainMenu;

	void SetPlayer(ThePlayer* player);

	bool Initialize();
	bool BeginRun();
	bool Load();

	void FixedUpdate();
	void GameInput();

private:
	bool GameEnded = false;
	Vector2 AdjustedFieldSize = {};

	ThePlayer* Player = {};
	TheMainMenu* Main = {};

	void ThePlayerHit();
	void EndTheGame();
	void NewGame();
	void GameInPlay();
	void GamePaused();
	void DisplayHighScores();
	void GameMainMenu();
};