#pragma once
#include "Globals.h"
#include "raygui.h"
#include "dark/style_dark.h"
#include "ThePlayer.h"

struct LoadedLineModels
{
	LineModel *Model = {};
	std::string Name = {};
	size_t IDNumber = 0;
};

class TheMainMenu : public Common
{
public:
	TheMainMenu();
	virtual ~TheMainMenu();

	void SetPlayer(ThePlayer* player);

	bool Initialize();
	bool Load();
	bool BeginRun();

	void Update();

private:
	bool TextBoxXEditMode = false;
	bool TextBoxYEditMode = false;
	bool TextBoxScaleEditMode = false;
	bool TextBoxPointEditMode = false;
	bool textBoxEditMode = false;
	bool ShowLoadTextInputBox = false;
	bool ShowSaveTextInputBox = false;
	bool LoadSceneButtonHit = false;
	bool SaveSceneButtonHit = false;
	bool CollisionCheckBox = false;
	bool MirrorCheckBox = false;

	size_t CursurIndex = 0;

	char TextBoxScale[16] = "1.0";
	char TextBoxXInput[16] = "0";
	char TextBoxYInput[16] = "0";
	char TextBoxPointIntput[16] = "0";

	char TextInput[256] = { 0 };

	char FileNameInput[256] = { 0 };

	const char* TextOkCancel = "Ok;Cancel";


	Rectangle TextInputBoxLocation = { 0, 0, 0, 0 };

	std::vector<Vector3> CursorModel = {};
	std::vector<Vector3> CrosshairModel = {};

	LineModel* Crosshair = {};
	LineModel* Cursor = {};
	ThePlayer* Player = {};

	std::vector<LoadedLineModels> LoadedModels = {};

	void DrawUI();
	void ResetModels();
	void ResetViewport();
	void NewScene();
	void MakeNewModel();
	void LoadScene();
	void SaveScene();
	void SaveAsInputBox();
	void SaveFile();
	void LoadInputBox();
	void MakeNewPoint();
	void MovePoint();
	void DeletePoint();
	void Mirror();
	void ApplyMirror();
	void Flip();
	void Center();
	void SetScale();
	void CursorUp();
	void CursorDown();
	void NextModel();
	void PreviousModel();
	void UpdateTextBoxesAndCursor();

	void DrawMirrorUI();

	bool CheckMirrorValidity();
};