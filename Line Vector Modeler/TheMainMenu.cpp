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

	NewScene();

	return false;
}

void TheMainMenu::Update()
{
	Common::Update();

	if (!ShowLoadSceneTextInput && !ShowLoadTextInput && !ShowSaveTextInput) KeybaordInput();

	DrawUI();


	if (ShowLoadTextInput) LoadModelInputBox();

	if (ShowLoadSceneTextInput) LoadSceneInputBox();

	if (ShowSaveTextInput) SaveAsInputBox();

	if (MirrorCheckBox) Mirror();
	else Player->MirrorModel->Enabled = false;

	if (ShowLoadErrorMessage) DrawLoadErrorMessage();

	if (!TextBoxXEditMode)
	{
		if (!TextBoxXInput[0]) return;

		float x = std::stof(TextBoxXInput);
		x += Player->Position.x;
		Cursor->X(x);
	}

	if (!TextBoxYEditMode)
	{
		if (!TextBoxYInput[0]) return;

		float y = std::stof(TextBoxYInput);
		y += Player->Position.y;
		Cursor->Y(y);
	}

	if (!TextBoxScaleEditMode)
	{
		if (!TextBoxScale[0]) return;

		float scale = std::stof(TextBoxScale);

		if (scale > 0.0f && scale < 100.0f) Player->Scale = scale;
	}

	if (TextBoxPointEditMode)
	{
		if (Player->GetLineModel().size() == 0) return;

		if (TextBoxPointIntput[0] == '\0') return;

		int point = std::stoi(TextBoxPointIntput);

		if (point >= 0 && point < Player->GetLineModel().size()) CursorIndex = point;
	}

	Player->HideCollision = !CollisionCheckBox;
}

void TheMainMenu::DrawUI()
{
	std::string sceneName = "Scene: ";
	sceneName.append(SceneFileNameInput);

	if (SceneFileNameInput[0]) GuiLabel({ 1050, 10, 200, 30 }, sceneName.c_str());

	std::string x = "X: " + std::to_string(Player->Position.x);
	std::string y = "Y: " + std::to_string(Player->Position.y);
	
	GuiLabel({ 500, 900, 200, 40 }, x.c_str());
	GuiLabel({ 500 + 200, 900, 200, 40 }, y.c_str());

	Rectangle buttonSave = { 50, 10, 85, 30 };
	Rectangle buttonLoad = { buttonSave.x + 100, 10, buttonSave.width, buttonSave.height };
	Rectangle buttonSaveAs = { buttonLoad.x + 100, 10, buttonSave.width + 10, buttonSave.height };
	Rectangle buttonNewModel = { buttonLoad.x + 220, 10, buttonSave.width + 40, buttonSave.height };
	Rectangle buttonReset = { buttonNewModel.x + 140, 10, buttonSave.width, buttonSave.height };
	Rectangle buttonNewScene = { buttonReset.x + 110, 10, buttonSaveAs.width + 25, buttonSave.height };
	Rectangle buttonLoadScene = { buttonNewScene.x + 130, 10, buttonNewScene.width + 10, buttonSave.height };
	Rectangle buttonSaveScene = { buttonLoadScene.x + 140, 10, buttonLoadScene.width, buttonSave.height };

	if (GuiButton(buttonLoad, GuiIconText(ICON_FILE_OPEN, "  Load"))) ShowLoadTextInput = true;

	if (Player->GetLineModel().size() > 0)
	{
		if (GuiButton(buttonSave, GuiIconText(ICON_FILE_SAVE, "  Save"))) SaveModel();
		if (GuiButton(buttonSaveAs, GuiIconText(ICON_FILE_SAVE, "Save as"))) ShowSaveTextInput = true;
		if (GuiButton(buttonNewModel, GuiIconText(ICON_FILE_NEW, "New Model"))) MakeNewModel();

		if (LoadedModels.size() < 1) if (GuiButton(buttonReset, GuiIconText(ICON_FILE_NEW, " Reset"))) ResetViewport();
	}

	if (GuiButton(buttonLoadScene, GuiIconText(ICON_FILE_OPEN, "Load scene"))) ShowLoadSceneTextInput = true;

	if (LoadedModels.size() > 1)
	{
		if (GuiButton(buttonNewScene, GuiIconText(ICON_FILE_NEW, "New scene"))) NewScene();

		if (!ModelFileName[0] == '\0')	if (GuiButton(buttonSaveScene, GuiIconText(ICON_FILE_SAVE, "Save scene"))) SaveScene();
	}

	Rectangle buttonNewPoint = { 1120, 200, 140, 30 };
	Rectangle buttonMovePoint = { buttonNewPoint.x, buttonNewPoint.y + 40, buttonNewPoint.width, 30 };
	Rectangle buttonDeletePoint = { buttonNewPoint.x - 20, buttonNewPoint.y + 700, 160, 30 };
	Rectangle buttonDeleteModel = { buttonDeletePoint.x, buttonDeletePoint.y - 40, buttonDeletePoint.width, buttonDeletePoint.height };

	Rectangle buttonUp = { 1120, 300, 120, 30 };
	Rectangle buttonDown = { buttonUp.x, buttonUp.y + 40, buttonUp.width, buttonUp.height };
	Rectangle buttonMirror = { buttonUp.x, buttonUp.y + 190, buttonUp.width + 30, buttonUp.height };
	Rectangle checkMirror = { buttonUp.x, buttonMirror.y - 40, 20, 20 };

	if (GuiButton(buttonNewPoint, GuiIconText(ICON_FILE_ADD, "  New point"))) MakeNewPoint();
	if (GuiButton(buttonMovePoint, GuiIconText(ICON_CURSOR_MOVE, "  Move point"))) MovePoint();
	if (GuiButton(buttonDeletePoint, GuiIconText(ICON_CROSS, "  Delete point"))) DeletePoint();

	if (LoadedModels.size() > 1) if (GuiButton(buttonDeleteModel, GuiIconText(ICON_FILE_DELETE, "Delete model"))) DeletePlayerFromScene();

	if (CheckMirrorValidity())
	{
		if (GuiButton(buttonMirror, GuiIconText(ICON_BOX, " Apply Mirror"))) ApplyMirror();
		GuiCheckBox(checkMirror, "Mirror", &MirrorCheckBox);
	}

	Rectangle buttonFlip = { buttonUp.x, buttonUp.y + 250, buttonUp.width, buttonUp.height };
	Rectangle buttonCenter = { buttonUp.x, buttonFlip.y + 40, buttonUp.width, buttonUp.height };
	Rectangle buttonSetOrigin = { buttonCenter.x, buttonCenter.y + 40, buttonCenter.width, buttonCenter.height };

	if (Player->GetLineModel().size() > 1)
	{
		if (GuiButton(buttonFlip, GuiIconText(ICON_REDO_FILL, "   Flip"))) Flip();
		if (GuiButton(buttonCenter, GuiIconText(ICON_ZOOM_CENTER, "  Center"))) Center();
		if (GuiButton(buttonSetOrigin, GuiIconText(ICON_TARGET_SMALL_FILL, "Set Origin"))) SetOrigin();
	}

	if (GuiButton(buttonUp, GuiIconText(ICON_ARROW_UP, "   Back"))) CursorUp();;
	if (GuiButton(buttonDown, GuiIconText(ICON_ARROW_DOWN, " Forward"))) CursorDown();

	Rectangle buttonScale = { 1150, 100, 100, 30 };
	Rectangle valueBoxScale = { buttonScale.x + 50, buttonScale.y - 35, 50, 30 };

	if (GuiButton(buttonScale, GuiIconText(ICON_CURSOR_SCALE_LEFT, "  Scale"))) SetScale();

	Rectangle labelScale = { valueBoxScale.x - 60, valueBoxScale.y, 50, valueBoxScale.height };
	GuiLabel(labelScale, "Scale:");

	if (GuiTextBox(valueBoxScale, TextBoxScale, 16, TextBoxScaleEditMode)) TextBoxScaleEditMode = !TextBoxScaleEditMode;

	Rectangle buttonZoomIn = { labelScale.x - 250, labelScale.y, 110, labelScale.height };
	Rectangle buttonZoomOut = { buttonZoomIn.x + 120, buttonZoomIn.y, buttonZoomIn.width, buttonZoomIn.height };
	Rectangle labelZoom = { buttonZoomIn.x, buttonZoomIn.y + 40, 150, buttonZoomIn.height };

	if (GuiButton(buttonZoomIn, GuiIconText(ICON_CAMERA, "Zoom In"))) ZoomIn();
	if (GuiButton(buttonZoomOut, GuiIconText(ICON_CAMERA, "Zoom Out"))) ZoomOut();

	std::string zoom = "Zoom: " + std::to_string(ZoomLevel) + "%";
	GuiLabel(labelZoom, zoom.c_str());

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

	Rectangle checkCollision = { 1130, buttonSetOrigin.y + 50, 20, 20 };

	GuiCheckBox(checkCollision, " Collision", &CollisionCheckBox);

	Rectangle buttonNextModel = { checkCollision.x, checkCollision.y + 50, 110, 30 };
	Rectangle buttonPreviousModel = { buttonNextModel.x, buttonNextModel.y + 40, buttonNextModel.width, buttonNextModel.height };

	std::string modelName = "Model: ";
	modelName.append(ModelFileName);
	if (ModelEdited) modelName.append("*");

	if (ModelFileName[0]) GuiLabel({ buttonPreviousModel.x - 150, buttonPreviousModel.y + 50, 200, 30 }, modelName.c_str());

	if (LoadedModels.size() > 1)
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

	Crosshair->SetParent(*Player);
}

void TheMainMenu::ResetViewport()
{
	ModelEdited = false;
	Player->Reset();
	Cursor->X(0.0f);
	Cursor->X(0.0f);

	TextCopy(ModelFileName, "\0");
	TextCopy(TextBoxScale, "1.0");
	TextCopy(TextBoxXInput, "0.0");
	TextCopy(TextBoxYInput, "0.0");
	TextCopy(TextBoxPointIntput, "1.0");

	CursorIndex = 0;
	ZoomLevel = 100;
	TheCamera.fovy = (float)GetScreenHeight();
	Cursor->Scale = 1.0f;
	CollisionCheckBox = false;
}

void TheMainMenu::NewScene()
{
	ResetViewport();
	ResetModels();
	TextCopy(SceneFileNameInput, "\0");
	TextCopy(ModelFileName, "\0");
	SceneSize = 1;
}

void TheMainMenu::MakeNewModel()
{
	if (!ModelFileName[0]) return;

	bool fileNotFound = true;

	for (auto &model : LoadedModels)
	{
		if (model.Name == ModelFileName) fileNotFound = false;
	}

	if (fileNotFound) AddPlayerToScene();
	else
	{
		LoadedModels[Player->ModelIndex].Model->Enabled = true;
		LoadedModels[Player->ModelIndex].Model->Position = Player->Position;
	}

	ModelEdited = false;
	Player->ClearModel();
	Player->Reset();
	SceneSize = LoadedModels.size();
	CursorIndex = 0;
	TextCopy(TextBoxScale, "1.0");
	TextCopy(TextBoxPointIntput, "1.0");
	TextCopy(ModelFileName, "\0");
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::LoadModel(std::string fileName)
{
	ModelEdited = false;
	Player->SetModel(CM.LoadAndGetLineModel(fileName));

	if (LoadedModels.size() > 0) AddPlayerToScene();

	SceneSize = LoadedModels.size();
	CursorIndex = Player->GetLineModel().size() - 1;
	Cursor->Position = Player->GetLineModel()[CursorIndex];
	TextCopy(TextBoxPointIntput, std::to_string(CursorIndex).c_str());
	TextCopy(TextBoxXInput, std::to_string(Cursor->Position.x).c_str());
	TextCopy(TextBoxYInput, std::to_string(Cursor->Position.y).c_str());
}

void TheMainMenu::LoadScene()
{
	if (FileExists(CM.GetSceneFileNameWithPath(SceneFileNameInput).c_str()))
	{
		std::string sceneSTR = LoadFileText(CM.GetSceneFileNameWithPath(SceneFileNameInput).c_str());
		ResetModels();
		ResetViewport();

		std::vector<Vector3> positions = CM.ConvertStringToPointsNew(sceneSTR);
		std::vector<std::string> names;
		std::string name;
		bool foundName = false;

		for (const auto& character : sceneSTR)
		{
			if (character == ':')
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

		bool fileTest = true;

		for (auto& name : names)
		{
			if (name.size() == 0) fileTest = false;
		}

		if (names.size() != positions.size() || !fileTest)
		{
			ShowLoadErrorMessage = true;
			TextCopy(MessageBoxText, "Invalid Scene File");
			return;
		}

		for (size_t i = 0; i < positions.size(); i++)  
		{
			LoadedModels.push_back(LoadedLineModels());
			LoadedModels.back().IDNumber = EM.AddLineModel(LoadedModels.back().Model = DBG_NEW LineModel(), CM.LoadAndGetLineModel(names[i]));
			LoadedModels.back().Model->Position = positions[i];
			LoadedModels.back().Model->HideCollision = true;
			LoadedModels.back().Model->ModelColor = BLUE;
			LoadedModels.back().Name = names[i];
		}

		SceneSize = LoadedModels.size();
		Player->ModelIndex = 0;
		Player->Enabled = true;
		Player->SetModel((LoadedModels[0].Model->GetLineModel()));
		Player->Position = LoadedModels[0].Model->Position;
		LoadedModels[0].Model->Enabled = false;
		CursorIndex = Player->GetLineModel().size() - 1;
		TextCopy(ModelFileName, LoadedModels[0].Name.c_str());
		TextCopy(TextBoxPointIntput, std::to_string(CursorIndex).c_str());
		TextCopy(TextBoxXInput, std::to_string(Cursor->Position.x).c_str());
		TextCopy(TextBoxYInput, std::to_string(Cursor->Position.y).c_str());
		UpdateTextBoxesAndCursor();
	}
	else
	{
		ShowLoadErrorMessage = true;
		TextCopy(MessageBoxText, "\0");
	}
}

void TheMainMenu::SaveAsInputBox()
{
	Player->Enabled = false;
	Crosshair->Enabled = false;
	Cursor->Enabled = false;

	for (auto model : LoadedModels)
	{
		model.Model->Enabled = false;
	}

	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(DARKGRAY, 0.8f));
	int result = GuiTextInputBox(TextInputBoxLocation, GuiIconText(ICON_FILE_SAVE, "Save file"), "File name:", TextOkCancel, TextInput, 255, NULL);

	if (result == 1)
	{
		bool fileError = true;

		if (SaveModelFile) fileError = FileExists(CM.GetLineModelFileNameWithPath(TextInput).c_str());
		else if (SaveSceneFile) fileError = FileExists(CM.GetSceneFileNameWithPath(TextInput).c_str());

		if (fileError)
		{
			ShowLoadErrorMessage = true;
			ShowSaveTextInput = false;

			std::string msg = TextInput;
			msg.append(" already exists.");

			TextCopy(MessageBoxText, msg.c_str());
		}
		else
		{
			if (SaveModelFile)
			{
				TextCopy(ModelFileName, TextInput);
				SaveModel();
				bool SaveModelFile = false;
			}
			else if (SaveSceneFile)
			{
				TextCopy(SceneFileNameInput, TextInput);
				SaveScene();
				bool SaveSceneFile = false;
			}
		}
	}

	if ((result == 0) || (result == 1) || (result == 2))
	{
		TextCopy(TextInput, "\0");
		ShowSaveTextInput = false;
		Player->Enabled = true;
		Cursor->Enabled = true;
		Crosshair->Enabled = true;

		for (auto model : LoadedModels)
		{
			model.Model->Enabled = true;
		}

		if (LoadedModels.size() > 1) LoadedModels[Player->ModelIndex].Model->Enabled = false;
	}
}

void TheMainMenu::SaveModel()
{
	if (Player->GetLineModel().size() < 1) return;

	// If there is no file name, open the input box to get file name, return.
	if (!ModelFileName[0])
	{
		SaveModelFile = true;
		ShowSaveTextInput = true;
		return;
	}

	if (!DirectoryExists("Models")) MakeDirectory("Models");

	CM.SaveLineModel(ModelFileName, Player->GetLineModel());

	if (LoadedModels.size() < SceneSize) AddPlayerToScene();

	SaveModelFile = false;
	ModelEdited = false;
}

void TheMainMenu::SaveScene()
{
	if (!ModelFileName[0] || Player->GetLineModel().size() < 1 || SceneSize != LoadedModels.size()) return;

	if (!SceneFileNameInput[0])
	{
		ShowSaveTextInput = true;
		SaveSceneFile = true;
		return;
	}

	std::vector<Scene> sceneModels;
	std::string sceneDataSTR;

	for (auto model : LoadedModels)
	{
		sceneModels.push_back(Scene());
		sceneModels.back().Position = model.Model->Position;
		sceneModels.back().Name = model.Name;
	}

	if (!DirectoryExists("Scenes")) MakeDirectory("Scenes");

	CM.SaveScene(SceneFileNameInput, sceneModels);

	SaveSceneFile = false;
}

void TheMainMenu::LoadModelInputBox()
{
	Player->Enabled = false;
	Crosshair->Enabled = false;
	Cursor->Enabled = false;

	for (auto model : LoadedModels)
	{
		model.Model->Enabled = false;
	}

	MirrorCheckBox = false;
	ModelEdited = false;

	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(DARKGRAY, 0.8f));
	int result = GuiTextInputBox(TextInputBoxLocation, GuiIconText(ICON_FILE_OPEN, "Load file"), "File name:", TextOkCancel, TextInput, 255, NULL);

	if (result == 1)
	{
		if (!FileExists(CM.GetLineModelFileNameWithPath(TextInput).c_str()))
		{
			ShowLoadErrorMessage = true;
			ShowLoadTextInput = false;
			TextCopy(MessageBoxText, TextInput);
		}
		else
		{
			TextCopy(ModelFileName, TextInput);
			LoadModel(ModelFileName);
		}

	}

	if ((result == 0) || (result == 1) || (result == 2))
	{
		ShowLoadTextInput = false;
		Player->Enabled = true;
		Cursor->Enabled = true;
		Crosshair->Enabled = true;
		CollisionCheckBox = false;
		TextCopy(TextInput, "\0");

		for (auto model : LoadedModels)
		{
			model.Model->Enabled = true;
		}

		if (LoadedModels.size() > 1) LoadedModels.back().Model->Enabled = false;
	}
}

void TheMainMenu::LoadSceneInputBox()
{
	Player->Enabled = false;
	Crosshair->Enabled = false;
	Cursor->Enabled = false;

	for (auto model : LoadedModels)
	{
		model.Model->Enabled = false;
	}

	MirrorCheckBox = false;

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

		ModelEdited = false;
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

		for (auto model : LoadedModels)
		{
			model.Model->Enabled = true;
		}

		if (LoadedModels.size() > 1) LoadedModels[Player->ModelIndex].Model->Enabled = false;
	}
}

void TheMainMenu::MakeNewPoint()
{
	ModelEdited = true;

	float x = std::stof(TextBoxXInput);
	float y = std::stof(TextBoxYInput);

	Player->AddPointAt({ x, y, 0 }, CursorIndex);
	if (CursorIndex < Player->GetLineModel().size() - 1) CursorIndex++;
	else CursorIndex = Player->GetLineModel().size() - 1;
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::MovePoint()
{
	ModelEdited = true;

	Player->MovePoint({ Cursor->Position.x - Player->Position.x, Cursor->Position.y - Player->Position.y, 0 }, CursorIndex);

}

void TheMainMenu::DeletePoint()
{
	ModelEdited = true;

	if (Player->GetLineModel().size() < 1)
	{
		CursorIndex = 0;
	}
	else
	{
		Player->DeletePoint(CursorIndex);

		if (CursorIndex > Player->GetLineModel().size() - 1) CursorIndex = Player->GetLineModel().size() - 1;
		else
		{
			if (CursorIndex > 0) CursorIndex--;
		}
	}

	UpdateTextBoxesAndCursor();
}

void TheMainMenu::Mirror()
{
	Player->Mirror();
}

void TheMainMenu::ApplyMirror()
{
	ModelEdited = true;
	MirrorCheckBox = false;
	Player->ApplyMirror();
	CursorIndex = Player->GetLineModel().size() - 1;
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::Flip()
{
	ModelEdited = true;
	Player->Flip();
}

void TheMainMenu::Center()
{
	ModelEdited = true;
	Player->Center();
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::SetOrigin()
{
	ModelEdited = true;
	Player->SetOrigin();
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::SetScale()
{
	ModelEdited = true;
	float scale = std::stof(TextBoxScale);

	if (scale > 0.0f && scale < 100.0f) Player->SetScale(std::stof(TextBoxScale));

	TextCopy(TextBoxScale, "1.0");
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::ZoomIn()
{
	TheCamera.fovy -= 100;

	if (TheCamera.fovy < 160) TheCamera.fovy = 160;

	ZoomLevel = 100 + (GetScreenHeight() - TheCamera.fovy) / 5.0f;

	float zoom = (float)ZoomLevel;
	float scale = powf(0.9975f, zoom);

	if (zoom > 160) scale = powf(0.996f, zoom);
	if (zoom > 200) scale = powf(0.995f, zoom);
	if (zoom > 220) scale = powf(0.994f, zoom);
	if (zoom > 240) scale = powf(0.993f, zoom);

	Cursor->Scale = scale;
}

void TheMainMenu::ZoomOut()
{
	TheCamera.fovy += 100;

	if (TheCamera.fovy > 1160) TheCamera.fovy = 1160;

	ZoomLevel = 100 + (GetScreenHeight() - TheCamera.fovy) / 5.0f;

	float zoom = (float)ZoomLevel;
	float scale = powf(0.9975f, zoom);

	if (zoom > 160) scale = powf(0.996f, zoom);
	if (zoom > 200) scale = powf(0.995f, zoom);
	if (zoom > 220) scale = powf(0.994f, zoom);
	if (zoom > 240) scale = powf(0.993f, zoom);

	Cursor->Scale = scale;
}

void TheMainMenu::CursorUp()
{
	if (Player->GetLineModel().size() < 1) return;

	if (CursorIndex > 0) CursorIndex--;
	else return;

	UpdateTextBoxesAndCursor();
}

void TheMainMenu::CursorDown()
{
	if (Player->GetLineModel().size() < 1) return;

	if (CursorIndex < Player->GetLineModel().size() - 1) CursorIndex++;
	else return;

	UpdateTextBoxesAndCursor();
}

void TheMainMenu::NextModel()
{
	if (LoadedModels.size() < SceneSize)
	{
		AddPlayerToScene();
	}
	else
	{
		LoadedModels[Player->ModelIndex].Model->SetModel(Player->GetLineModel());
		LoadedModels[Player->ModelIndex].Model->Position = Player->Position;
		LoadedModels[Player->ModelIndex].Model->Position.z = 0.0f;
		LoadedModels[Player->ModelIndex].Name = ModelFileName;
		LoadedModels[Player->ModelIndex].Edited = ModelEdited;
	}

	if (LoadedModels.size() < 2) return;

	if (Player->ModelIndex > LoadedModels.size() - 2)
	{
		LoadedModels.back().Model->Enabled = true;
		Player->ModelIndex = 0;
	}
	else
	{
		LoadedModels[Player->ModelIndex].Model->Enabled = true;
		Player->ModelIndex++;
	}

	Player->SetModel(LoadedModels[Player->ModelIndex].Model->GetLineModel());
	Player->Position = LoadedModels[Player->ModelIndex].Model->Position;
	ModelEdited = LoadedModels[Player->ModelIndex].Edited;
	CursorIndex = Player->GetLineModel().size() - 1;
	LoadedModels[Player->ModelIndex].Model->Enabled = false;
	TextCopy(ModelFileName, LoadedModels[Player->ModelIndex].Name.c_str());
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::PreviousModel()
{
	if (LoadedModels.size() < SceneSize)
	{
		AddPlayerToScene();
	}
	else
	{
		LoadedModels[Player->ModelIndex].Model->SetModel(Player->GetLineModel());
		LoadedModels[Player->ModelIndex].Model->Position = Player->Position;
		LoadedModels[Player->ModelIndex].Model->Position.z = 0.0f;
		LoadedModels[Player->ModelIndex].Name = ModelFileName;
		LoadedModels[Player->ModelIndex].Edited = ModelEdited;
	}

	if (LoadedModels.size() < 2) return;

	if (Player->ModelIndex < 1)
	{
		LoadedModels[0].Model->Enabled = true;
		Player->ModelIndex = LoadedModels.size() - 1;
	}
	else
	{
		LoadedModels[Player->ModelIndex].Model->Enabled = true;
		Player->ModelIndex--;
	}

	Player->SetModel(LoadedModels[Player->ModelIndex].Model->GetLineModel());
	Player->Position = LoadedModels[Player->ModelIndex].Model->Position;
	ModelEdited = LoadedModels[Player->ModelIndex].Edited;
	CursorIndex = Player->GetLineModel().size() - 1;
	LoadedModels[Player->ModelIndex].Model->Enabled = false;
	TextCopy(ModelFileName, LoadedModels[Player->ModelIndex].Name.c_str());
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::AddPlayerToScene()
{
	LoadedModels.push_back(LoadedLineModels());
	LoadedModels.back().IDNumber = EM.AddLineModel(LoadedModels.back().Model = DBG_NEW LineModel(), Player->GetLineModel());
	LoadedModels.back().Model->HideCollision = true;
	LoadedModels.back().Model->ModelColor = BLUE;
	LoadedModels.back().Model->SetModel(Player->GetLineModel());
	LoadedModels.back().Model->Position = Player->Position;
	LoadedModels.back().Model->Position.z = 0.0f;
	LoadedModels.back().Name = ModelFileName;
	SceneSize = LoadedModels.size();
	Player->ModelIndex = LoadedModels.size() - 1;
}

void TheMainMenu::DeletePlayerFromScene()
{
	if (LoadedModels.size() < 2) return;

	size_t modelIndex = Player->ModelIndex;

	LoadedModels[modelIndex].Model->Enabled = false;

	if (Player->ModelIndex < 1)
	{
		EM.DeleteEntity(LoadedModels[modelIndex].IDNumber);
		LoadedModels.erase(LoadedModels.begin());
		modelIndex = LoadedModels.size() - 1;
	}
	else
	{
		EM.DeleteEntity(LoadedModels[modelIndex].IDNumber);
		LoadedModels.erase(LoadedModels.begin() + modelIndex);
		modelIndex--;
	}

	for (size_t i = 0; i < LoadedModels.size(); i++) LoadedModels[i].IDNumber--;

	Player->SetModel(LoadedModels[modelIndex].Model->GetLineModel());
	Player->Position = LoadedModels[modelIndex].Model->Position;
	Player->Position.z = -10.0f;
	Player->ModelIndex = modelIndex;
	TextCopy(ModelFileName, LoadedModels[modelIndex].Name.c_str());
	CursorIndex = Player->GetLineModel().size() - 1;
	SceneSize--;
	UpdateTextBoxesAndCursor();
}

void TheMainMenu::UpdateTextBoxesAndCursor()
{
	if (Player->GetLineModel().size() < 1)
	{
		TextCopy(TextBoxXInput, "0");
		TextCopy(TextBoxYInput, "0");
		TextCopy(TextBoxPointIntput, "0");

		return;
	}

	TextCopy(TextBoxXInput, std::to_string(Player->GetLineModel()[CursorIndex].x).c_str());
	TextCopy(TextBoxYInput, std::to_string(Player->GetLineModel()[CursorIndex].y).c_str());
	TextCopy(TextBoxPointIntput, std::to_string(CursorIndex).c_str());
}

void TheMainMenu::UpdateCursor()
{
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

void TheMainMenu::KeybaordInput()
{
	if (IsKeyPressed(KEY_D))
	{
		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
		{
			Player->Position.x += 9.0f;
		}

		Player->Position.x += 1.0f;
		UpdateTextBoxesAndCursor();
	}
	else if (IsKeyPressed(KEY_A))
	{
		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
		{
			Player->Position.x -= 9.0f;

		}

		Player->Position.x -= 1.0f;
		UpdateTextBoxesAndCursor();
	}
	else
	{
	}

	if (IsKeyPressed(KEY_W))
	{
		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
		{
			Player->Position.y -= 9.0f;
		}

		Player->Position.y -= 1.0f;
		UpdateTextBoxesAndCursor();
	}
	else if (IsKeyPressed(KEY_S))
	{
		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
		{
			Player->Position.y += 9.0f;
		}

		Player->Position.y += 1.0f;
		UpdateTextBoxesAndCursor();
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
