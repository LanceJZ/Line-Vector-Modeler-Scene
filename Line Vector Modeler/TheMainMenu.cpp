#define _CRT_SECURE_NO_WARNINGS
#include "TheMainMenu.h"

TheMainMenu::TheMainMenu()
{
}

TheMainMenu::~TheMainMenu()
{
}

void TheMainMenu::SetPlayer(ThePlayer* player)
{
	Player = player;
}

bool TheMainMenu::Initialize()
{
	Common::Initialize();


	return false;
}

bool TheMainMenu::Load()
{
	CrosshairModel = CM.LoadAndGetLineModel("Cross");
	CursorModel = CM.LoadAndGetLineModel("Cursor");

	ResetModels();

	return true;
}

bool TheMainMenu::BeginRun()
{
	Common::BeginRun();

	Cursor->BeginRun();
	Crosshair->BeginRun();

	GuiLoadStyleDark();
	GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

	TextInputBoxLocation = { (float)GetScreenWidth() / 2 - 120, (float)GetScreenHeight() / 2 - 60, 240, 140 };

	GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
	GuiSetStyle(DEFAULT, TEXT_SIZE, GuiGetFont().baseSize * 1.5f);

	return false;
}

void TheMainMenu::Update()
{
	Common::Update();

	DrawUI();


	if (ShowLoadTextInput) LoadModelInputBox();

	if (ShowLoadSceneTextInput) LoadSceneInputBox();

	if (ShowSaveTextInput) SaveAsInputBox();

	if (MirrorCheckBox) Mirror();

	if (ShowLoadErrorMessage) DrawLoadErrorMessage();

	if (!TextBoxXEditMode)
	{
		if (TextBoxXInput[0] == '\0') return;

		float x = std::stof(TextBoxXInput);

		if (x > -1000.0f && x < 1000.0f) Cursor->Position.x = x;

	}

	if (!TextBoxYEditMode)
	{
		if (TextBoxYInput[0] == '\0') return;

		float y = std::stof(TextBoxYInput);

		if (y > -1000.0f && y < 1000.0f) Cursor->Position.y = y;

	}

	if (!TextBoxScaleEditMode)
	{
		if (TextBoxScale[0] == '\0') return;

		float scale = std::stof(TextBoxScale);

		if (scale > 0.0f && scale < 100.0f) Player->Scale = scale;
	}

	if (TextBoxPointEditMode)
	{
		if (Player->GetLineModel().size() == 0) return;

		if (TextBoxPointIntput[0] == '\0') return;

		int point = std::stoi(TextBoxPointIntput);

		if (point >= 0 && point < Player->GetLineModel().size()) CursurIndex = point;
	}

	Player->HideCollision = !CollisionCheckBox;
}

void TheMainMenu::DrawUI()
{
	std::string x = "X: " + std::to_string(Crosshair->Position.x);
	std::string y = "Y: " + std::to_string(Crosshair->Position.y);
	
	GuiLabel({ 500, 900, 200, 40 }, x.c_str());
	GuiLabel({ 500 + 200, 900, 200, 40 }, y.c_str());

	if (ModelFileNameInput[0]) GuiLabel({ 1150, 10, 200, 30 }, ModelFileNameInput);

	Rectangle buttonSave = { 100, 10, 90, 30 };
	Rectangle buttonLoad = { buttonSave.x + 100, 10, buttonSave.width, buttonSave.height };
	Rectangle buttonSaveAs = { buttonLoad.x + 100, 10, buttonSave.width + 20, buttonSave.height };
	Rectangle buttonNew = { buttonLoad.x + 250, 10, buttonSave.width - 10, buttonSave.height };
	Rectangle buttonReset = { buttonNew.x + 90, 10, buttonNew.width, buttonSave.height };
	Rectangle buttonNewScene = { buttonReset.x + 100, 10, buttonSaveAs.width + 10, buttonSave.height };
	Rectangle buttonLoadScene = { buttonNewScene.x + 130, 10, buttonNewScene.width + 10, buttonSave.height };
	Rectangle buttonSaveScene = { buttonLoadScene.x + 140, 10, buttonLoadScene.width, buttonSave.height };

	if (GuiButton(buttonLoad, GuiIconText(ICON_FILE_OPEN, "  Load"))) ShowLoadTextInput = true;

	if (Player->GetLineModel().size() > 0)
	{
		if (GuiButton(buttonSave, GuiIconText(ICON_FILE_SAVE, "  Save"))) SaveModel();
		if (GuiButton(buttonSaveAs, GuiIconText(ICON_FILE_SAVE, " Save as"))) ShowSaveTextInput = true;
		if (GuiButton(buttonNew, GuiIconText(ICON_FILE_NEW, "  New"))) MakeNewModel();
		if (GuiButton(buttonReset, GuiIconText(ICON_FILE_NEW, "Reset"))) ResetViewport();
	}

	if (GuiButton(buttonLoadScene, GuiIconText(ICON_FILE_OPEN, "Load scene"))) ShowLoadSceneTextInput = true;

	if (LoadedModels.size() > 0)
	{
		if (GuiButton(buttonNewScene, GuiIconText(ICON_FILE_NEW, "New scene"))) NewScene();

		if (!ModelFileNameInput[0] == '\0')	if (GuiButton(buttonSaveScene, GuiIconText(ICON_FILE_SAVE, "Save scene"))) SaveScene();
	}

	Rectangle buttonNewPoint = { 1120, 200, 140, 30 };
	Rectangle buttonMovePoint = { buttonNewPoint.x, buttonNewPoint.y + 40, buttonNewPoint.width, 30 };
	Rectangle buttonDeletePoint = { buttonNewPoint.x - 20, buttonNewPoint.y + 700, 160, 30 };
	//Rectangle buttonNewLine = { buttonNewPoint.x, buttonNewPoint.y + 150, 125,30 };

	Rectangle buttonUp = { 1120, 300, 120, 30 };
	Rectangle buttonDown = { buttonUp.x, buttonUp.y + 40, buttonUp.width, buttonUp.height };
	Rectangle buttonMirror = { buttonUp.x, buttonUp.y + 190, buttonUp.width + 30, buttonUp.height };
	Rectangle checkMirror = { buttonUp.x, buttonMirror.y - 40, 20, 20 };
	Rectangle buttonFlip = { buttonUp.x, buttonUp.y + 240, buttonUp.width, buttonUp.height };
	Rectangle buttonCenter = { buttonUp.x, buttonUp.y + 290, buttonUp.width, buttonUp.height };

	if (GuiButton(buttonNewPoint, GuiIconText(ICON_FILE_ADD, "  New point"))) MakeNewPoint();
	if (GuiButton(buttonMovePoint, GuiIconText(ICON_CURSOR_MOVE, "  Move point"))) MovePoint();
	if (GuiButton(buttonDeletePoint, GuiIconText(ICON_CROSS, "  Delete point"))) DeletePoint();

	if (CheckMirrorValidity())
	{
		if (GuiButton(buttonMirror, GuiIconText(ICON_BOX, " Apply Mirror"))) ApplyMirror();
		GuiCheckBox(checkMirror, "Mirror", &MirrorCheckBox);
	}

	if (GuiButton(buttonFlip, GuiIconText(ICON_REDO_FILL, "   Flip"))) Flip();
	if (GuiButton(buttonCenter, GuiIconText(ICON_ZOOM_CENTER, "  Center"))) Center();
	//if (GuiButton(buttonNewLine, GuiIconText(ICON_CURSOR_SCALE_FILL, "  New line"))) {}


	if (GuiButton(buttonUp, GuiIconText(ICON_ARROW_UP, "   Back"))) CursorUp();;
	if (GuiButton(buttonDown, GuiIconText(ICON_ARROW_DOWN, " Forward"))) CursorDown();

	Rectangle buttonScale = { 1150, 100, 100, 30 };
	Rectangle valueBoxScale = { buttonScale.x + 50, buttonScale.y - 35, 50, 30 };

	if (GuiButton(buttonScale, GuiIconText(ICON_CURSOR_SCALE_LEFT, "  Scale"))) SetScale();

	Rectangle labelScale = { valueBoxScale.x - 60, valueBoxScale.y, 50, valueBoxScale.height };
	GuiLabel(labelScale, "Scale:");

	if (GuiTextBox(valueBoxScale, TextBoxScale, 16, TextBoxScaleEditMode)) TextBoxScaleEditMode = !TextBoxScaleEditMode;

	Rectangle textBoxX = { 1000, buttonNewPoint.y - 50, 120, 30 };
	Rectangle textBoxY = { textBoxX.x + 150, textBoxX.y, textBoxX.width, textBoxX.height };
	Rectangle labelX = { textBoxX.x - 20, textBoxX.y, 50, textBoxX.height };
	Rectangle labelY = { textBoxY.x - 20, textBoxY.y, 50, textBoxY.height };

	GuiLabel(labelX, "X:");
	GuiLabel(labelY, "Y:");

	if (GuiTextBox(textBoxX, TextBoxXInput, 16, TextBoxXEditMode)) TextBoxXEditMode = !TextBoxXEditMode;
	if (GuiTextBox(textBoxY, TextBoxYInput, 16, TextBoxYEditMode)) TextBoxYEditMode = !TextBoxYEditMode;

	Rectangle labelPoint = { 1130, buttonDown.y + 40, 100, 30 };
	Rectangle valueBoxPoint = { labelPoint.x + 60, labelPoint.y, 50, 30 };

	GuiLabel(labelPoint, "Point:");
	if (GuiTextBox(valueBoxPoint, TextBoxPointIntput, 16, TextBoxPointEditMode)) TextBoxPointEditMode = !TextBoxPointEditMode;

	Rectangle checkCollision = { 1130, buttonCenter.y + 50, 20, 20 };

	GuiCheckBox(checkCollision, " Collision", &CollisionCheckBox);

	Rectangle buttonNextModel = { checkCollision.x, checkCollision.y + 50, 110, 30 };
	Rectangle buttonPreviousModel = { buttonNextModel.x, buttonNextModel.y + 40, buttonNextModel.width, buttonNextModel.height };

	
	if (LoadedModels.size() > 0)
	{
		if (GuiButton(buttonNextModel, GuiIconText(ICON_ARROW_RIGHT, "  Next"))) NextModel();
		if (GuiButton(buttonPreviousModel, GuiIconText(ICON_ARROW_LEFT, "Previous"))) PreviousModel();
	}
}

void TheMainMenu::ResetModels()
{
	LoadedModels.clear();

	EM.DeleteEntities();

	EM.AddLineModel(Crosshair = DBG_NEW LineModel());
	EM.AddLineModel(Cursor = DBG_NEW LineModel());
	EM.AddLineModel(Player = DBG_NEW ThePlayer());

	Crosshair->SetModel(CrosshairModel);
	Cursor->SetModel(CursorModel);

	Crosshair->Initialize();
	Cursor->Initialize();

	Crosshair->HideCollision = true;
	Crosshair->Alpha = 128;
	Crosshair->ModelColor = GRAY;

	Cursor->HideCollision = true;
	Cursor->Alpha = 200;
	Cursor->ModelColor = LIGHTGRAY;
}

void TheMainMenu::ResetViewport()
{
	Player->Reset();
	Cursor->X(0.0f);
	Cursor->X(0.0f);

	TextCopy(ModelFileNameInput, "\0");
	TextCopy(TextBoxScale, "1.0");
	TextCopy(TextBoxXInput, "0.0");
	TextCopy(TextBoxYInput, "0.0");
	TextCopy(TextBoxPointIntput, "1.0");

	CursurIndex = 0;
	
	CollisionCheckBox = false;
}

void TheMainMenu::NewScene()
{
	ResetViewport();
	ResetModels();
}

void TheMainMenu::MakeNewModel()
{
	if (ModelFileNameInput[0] == '\0') return;

	LoadedModels.push_back(LoadedLineModels());
	LoadedModels.back().IDNumber = EM.AddLineModel(LoadedModels.back().Model = DBG_NEW LineModel(), Player->GetLineModel());
	LoadedModels.back().Model->Position = Player->Position;
	LoadedModels.back().Model->HideCollision = true;
	LoadedModels.back().Model->ModelColor = BLUE;
	LoadedModels.back().Name = ModelFileNameInput;
	ResetViewport();
}

void TheMainMenu::LoadModel(std::string fileName)
{
	ResetViewport();
	Player->SetModel(CM.LoadAndGetLineModel(fileName));
	CursurIndex = Player->GetLineModel().size() - 1;
	Cursor->Position = Player->GetLineModel()[CursurIndex];
	TextCopy(TextBoxPointIntput, std::to_string(CursurIndex).c_str());
	TextCopy(TextBoxXInput, std::to_string(Cursor->Position.x).c_str());
	TextCopy(TextBoxYInput, std::to_string(Cursor->Position.y).c_str());
}

void TheMainMenu::LoadScene()
{
	std::string path = "Scenes/";
	std::string fileNameSTR = path;
	fileNameSTR.append(SceneFileNameInput);
	fileNameSTR.append(".scn");

	if (FileExists(fileNameSTR.c_str()))
	{
		std::string sceneSTR = LoadFileText(fileNameSTR.c_str());
		ResetModels();
		ResetViewport();

		std::vector<Vector3> positions;

		positions = CM.ConvertStringToPointsNew(sceneSTR);

		std::vector<std::string> names;
		std::string name;
		bool foundName = false;

		for (const auto& character : sceneSTR)
		{
			if (character == '}')
			{
				names.push_back(name);
				name.clear();
				foundName = false;
			}

			if (foundName)
			{
				name.append(1, character);
			}

			if (character == ';') foundName = true;
		}

		if (names.size() != positions.size())
		{
			ShowLoadErrorMessage = true;
			TextCopy(MessageBoxText, "Invalid Scene File");
			return;
		}

		Player->SetModel(CM.LoadAndGetLineModel(names[0]));
		Player->Position = positions[0];
		TextCopy(ModelFileNameInput, names[0].c_str());

		CursurIndex = Player->GetLineModel().size() - 1;
		Cursor->Position = Player->GetLineModel()[CursurIndex];
		TextCopy(TextBoxPointIntput, std::to_string(CursurIndex).c_str());
		TextCopy(TextBoxXInput, std::to_string(Cursor->Position.x).c_str());
		TextCopy(TextBoxYInput, std::to_string(Cursor->Position.y).c_str());

		for (size_t i = 1; i < positions.size(); i++)  
		{
			LoadedModels.push_back(LoadedLineModels());
			LoadedModels.back().IDNumber = EM.AddLineModel(LoadedModels.back().Model = DBG_NEW LineModel(), CM.LoadAndGetLineModel(names[i]));
			LoadedModels.back().Model->Position = positions[i];
			LoadedModels.back().Model->HideCollision = true;
			LoadedModels.back().Model->ModelColor = BLUE;
			LoadedModels.back().Name = names[i];
		}
	}
	else
	{
		ShowLoadErrorMessage = true;
		TextCopy(MessageBoxText, SceneFileNameInput);
	}
}

void TheMainMenu::SaveAsInputBox()
{
	Player->Enabled = false;
	Crosshair->Enabled = false;
	Cursor->Enabled = false;

	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(DARKGRAY, 0.8f));
	int result = GuiTextInputBox(TextInputBoxLocation, GuiIconText(ICON_FILE_SAVE, "Save file"), "File name:", TextOkCancel, TextInput, 255, NULL);

	if (result == 1)
	{
		if (SaveModelFile)
		{
			TextCopy(ModelFileNameInput, TextInput);
			SaveModel();
			SaveModelFile = false;
		}
		else if (SaveSceneFile)
		{
			TextCopy(SceneFileNameInput, TextInput);
			SaveScene();
			SaveSceneFile = false;
		}
	}

	if ((result == 0) || (result == 1) || (result == 2))
	{
		TextCopy(TextInput, "\0");
		ShowSaveTextInput = false;
		Player->Enabled = true;
		Cursor->Enabled = true;
		Crosshair->Enabled = true;
	}
}

void TheMainMenu::SaveModel()
{
	if (Player->GetLineModel().size() < 1) return;

	// If there is no file name, open the input box to get file name, return.
	if (!ModelFileNameInput[0])
	{
		ShowSaveTextInput = true;
		SaveModelFile = true;
		SaveSceneFile = false;
		return;
	}

	std::string path = "Models/";
	std::string nameVEC = path;
	nameVEC.append(ModelFileNameInput);
	nameVEC.append(".vec");

	char * fileName = new char[nameVEC.length() + 1]; 
	strcpy(fileName, nameVEC.c_str());

	std::string modelPointsSTR;

	for (int i = 0; i < Player->GetLineModel().size(); i++)
	{
		modelPointsSTR.append("{X:");
		modelPointsSTR.append(std::to_string(Player->GetLineModel()[i].x));
		modelPointsSTR.append(" Y:");
		modelPointsSTR.append(std::to_string(Player->GetLineModel()[i].y));
		modelPointsSTR.append(" Z:");
		modelPointsSTR.append(std::to_string(Player->GetLineModel()[i].z));
		modelPointsSTR.append("}");
	}

	char * linePoints = new char[modelPointsSTR.length() + 1];
	strcpy(linePoints, modelPointsSTR.c_str());

	SaveFileText(fileName, linePoints);
	SaveModelFile = false;
}

void TheMainMenu::SaveScene()
{
	if (ModelFileNameInput[0] || Player->GetLineModel().size() < 1) return;

	if (!SceneFileNameInput[0])
	{
		ShowSaveTextInput = true;
		SaveSceneFile = true;
		SaveModelFile = false;
		return;
	}

	std::string path = "Scenes/";
	std::string nameVEC = path;
	nameVEC.append(SceneFileNameInput);
	nameVEC.append(".scn");

	char * fileName = new char[nameVEC.length() + 1]; 
	strcpy(fileName, nameVEC.c_str());

	std::string sceneDataSTR;

	sceneDataSTR.append("{(X:");
	sceneDataSTR.append(std::to_string(Player->Position.x));
	sceneDataSTR.append(",Y:");
	sceneDataSTR.append(std::to_string(Player->Position.y));
	sceneDataSTR.append(",Z:");
	sceneDataSTR.append(std::to_string(Player->Position.z));
	sceneDataSTR.append(") Name;");
	sceneDataSTR.append(ModelFileNameInput);
	sceneDataSTR.append("}");
	
	for (int i = 0; i < LoadedModels.size(); i++)
	{
		sceneDataSTR.append("{(X:");
		sceneDataSTR.append(std::to_string(LoadedModels[i].Model->Position.x));
		sceneDataSTR.append(",Y:");
		sceneDataSTR.append(std::to_string(LoadedModels[i].Model->Position.y));
		sceneDataSTR.append(",Z:");
		sceneDataSTR.append(std::to_string(LoadedModels[i].Model->Position.z));
		sceneDataSTR.append(") Name:");
		sceneDataSTR.append(LoadedModels[i].Name);
		sceneDataSTR.append("}");
	}

	char * sceneData = new char[sceneDataSTR.length() + 1];
	strcpy(sceneData, sceneDataSTR.c_str());

	SaveFileText(fileName, sceneData);
	SaveSceneFile = false;
}

void TheMainMenu::LoadModelInputBox()
{
	Player->Enabled = false;
	Crosshair->Enabled = false;
	Cursor->Enabled = false;

	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(DARKGRAY, 0.8f));
	int result = GuiTextInputBox(TextInputBoxLocation, GuiIconText(ICON_FILE_OPEN, "Load file"), "File name:", TextOkCancel, TextInput, 255, NULL);

	if (result == 1)
	{
		std::string nameVEC = "Models/";
		nameVEC.append(TextInput);
		nameVEC.append(".vec");

		if (!FileExists(nameVEC.c_str()))
		{
			ShowLoadErrorMessage = true;
			ShowLoadTextInput = false;
			TextCopy(MessageBoxText, TextInput);
			TextCopy(TextInput, "\0");
			return;
		}

		TextCopy(ModelFileNameInput, TextInput);
		LoadModel(ModelFileNameInput);
	}

	if ((result == 0) || (result == 1) || (result == 2))
	{
		ShowLoadTextInput = false;
		TextCopy(TextInput, "\0");
		Player->Enabled = true;
		Cursor->Enabled = true;
		Crosshair->Enabled = true;
		CollisionCheckBox = false;
	}
}

void TheMainMenu::LoadSceneInputBox()
{
	Player->Enabled = false;
	Crosshair->Enabled = false;
	Cursor->Enabled = false;

	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(DARKGRAY, 0.8f));
	int result = GuiTextInputBox(TextInputBoxLocation, GuiIconText(ICON_FILE_OPEN, "Load file"), "File name:", TextOkCancel, TextInput, 255, NULL);

	if (result == 1)
	{
		std::string nameVEC = "Scenes/";
		nameVEC.append(TextInput);
		nameVEC.append(".scn");

		if (!FileExists(nameVEC.c_str()))
		{
			ShowLoadErrorMessage = true;
			ShowLoadTextInput = false;
			TextCopy(MessageBoxText, TextInput);
			TextCopy(TextInput, "\0");
			return;
		}

		ResetViewport();
		TextCopy(SceneFileNameInput, TextInput);
		LoadScene();
	}

	if ((result == 0) || (result == 1) || (result == 2))
	{
		ShowLoadSceneTextInput = false;
		TextCopy(TextInput, "\0");
		Player->Enabled = true;
		Cursor->Enabled = true;
		Crosshair->Enabled = true;
		CollisionCheckBox = false;
	}
}

void TheMainMenu::MakeNewPoint()
{
	Player->AddPointAt({ Cursor->Position.x, Cursor->Position.y, 0 }, CursurIndex);
	if (CursurIndex < Player->GetLineModel().size() - 1) CursurIndex++;
	else CursurIndex = Player->GetLineModel().size() - 1;
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::MovePoint()
{
	Player->MovePoint({ Cursor->Position.x, Cursor->Position.y, 0 }, CursurIndex);
}

void TheMainMenu::DeletePoint()
{
	if (Player->GetLineModel().size() < 1)
	{
		CursurIndex = 0;
		Cursor->Position = { 0, 0, 0 };
		UpdateTextBoxesAndCursor();
		return;
	}

	Player->DeletePoint(CursurIndex);

	if (CursurIndex > Player->GetLineModel().size() - 1) CursurIndex = Player->GetLineModel().size() - 1;
	else CursurIndex--;

	UpdateTextBoxesAndCursor();
}

void TheMainMenu::Mirror()
{
	Player->Mirror();
}

void TheMainMenu::ApplyMirror()
{
	MirrorCheckBox = false;
	Player->ApplyMirror();
	CursurIndex = Player->GetLineModel().size() - 1;
	Cursor->Position = Player->GetLineModel()[CursurIndex];
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::Flip()
{
	Player->Flip();
}

void TheMainMenu::Center()
{
	Player->Center();
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::SetScale()
{
	float scale = std::stof(TextBoxScale);

	if (scale > 0.0f && scale < 100.0f) Player->SetScale(std::stof(TextBoxScale));

	TextCopy(TextBoxScale, "1.0");
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::CursorUp()
{
	if (Player->GetLineModel().size() < 1) return;

	if (CursurIndex > 0) CursurIndex--;
	else return;

	UpdateTextBoxesAndCursor();
}

void TheMainMenu::CursorDown()
{
	if (Player->GetLineModel().size() < 1) return;

	if (CursurIndex < Player->GetLineModel().size() - 1) CursurIndex++;
	else return;

	UpdateTextBoxesAndCursor();
}

void TheMainMenu::NextModel()
{
}

void TheMainMenu::PreviousModel()
{
}

void TheMainMenu::UpdateTextBoxesAndCursor()
{
	if (Player->GetLineModel().size() > 0) Cursor->Position = Player->Position + Player->GetLineModel()[CursurIndex] * Player->Scale;
	TextCopy(TextBoxXInput, std::to_string(Cursor->Position.x).c_str());
	TextCopy(TextBoxYInput, std::to_string(Cursor->Position.y).c_str());	
	TextCopy(TextBoxPointIntput, std::to_string(CursurIndex).c_str());
	Crosshair->Position = Player->Position;
}

void TheMainMenu::DrawMirrorUI()
{

}

void TheMainMenu::DrawLoadErrorMessage()
{
	std::string msg = "The file (";
	msg.append(MessageBoxText);
	msg.append(") you attempted to load was not found.");

	int size = msg.size();

	int result = GuiMessageBox({ (float)GetScreenWidth() / 4, (float)GetScreenHeight() / 2.5f, (float)size * 10, 100 }, "File Not found.", msg.c_str(), "Oh no;Not again");

	if (result >= 0)
	{
		ShowLoadErrorMessage = false;
		Player->Enabled = true;
		Cursor->Enabled = true;
		Crosshair->Enabled = true;
	}
}

bool TheMainMenu::CheckMirrorValidity()
{
	if (Player->GetLineModel().size() < 1) return false;

	// Make sure Y does not go below 0.
	for (auto point : Player->GetLineModel())
	{
		if (point.y < 0) return false;
	}

	return true;
}
