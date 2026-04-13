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
	bool Edited = false;
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
	bool ShowLoadTextInput = false;
	bool ShowSaveTextInput = false;
	bool ShowLoadErrorMessage = false;
	bool ShowLoadSceneTextInput = false;
	bool CollisionCheckBox = false;
	bool MirrorCheckBox = false;
	bool LoadModelFile = false;
	bool LoadSceneFile = false;
	bool SaveModelFile = false;
	bool SaveSceneFile = false;
	bool ModelEdited = false;

	size_t CursorIndex = 0;
	size_t SceneSize = 0;

	int ZoomLevel = 100;

	char TextBoxScale[16] = "1.0";
	char TextBoxXInput[16] = "0";
	char TextBoxYInput[16] = "0";
	char TextBoxPointIntput[16] = "0";

	char TextInput[256] = { 0 };

	char ModelFileName[256] = { 0 };
	char SceneFileNameInput[256] = { 0 };
	char MessageBoxText[256] = { 0 };

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
	void LoadModel(std::string fileName);
	void LoadScene();
	void SaveAsInputBox();
	void SaveModel();
	void SaveScene();
	void LoadModelInputBox();
	void LoadSceneInputBox();
	void MakeNewPoint();
	void MovePoint();
	void DeletePoint();
	void Mirror();
	void ApplyMirror();
	void Flip();
	void Center();
	void SetOrigin();
	void SetScale();
	void ZoomIn();
	void ZoomOut();
	void CursorUp();
	void CursorDown();
	void NextModel();
	void PreviousModel();
	void AddPlayerToScene();
	void DeletePlayerFromScene();
	void UpdateTextBoxesAndCursor();
	void UpdateCursor();

	void DrawMirrorUI();
	void DrawLoadErrorMessage();
	void KeybaordInput();

	bool CheckMirrorValidity();
};