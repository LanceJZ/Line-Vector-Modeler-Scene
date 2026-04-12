#pragma once
#include "Globals.h"
#include "LineModel.h"

class ThePlayer : public LineModel
{
public:
	ThePlayer();
	virtual ~ThePlayer();

	size_t ModelIndex = 0;
	LineModel* MirrorModel = nullptr;

	bool Initialize();
	bool BeginRun();

	void Input();
	void Update(float deltaTime);
	void FixedUpdate(float deltaTime);
	void Draw3D();
	void Reset();
	void Spawn();
	void NewGame();

	void ClearModel();

	void AddPoint(Vector3 point);
	void AddPointAt(Vector3 point, size_t index);
	void MovePoint(Vector3 point, size_t index);
	void DeletePoint(size_t index);
	void Mirror();
	void ApplyMirror();
	void Flip();
	void Center();
	void SetOrigin();
	void SetScale(float scale);

private:

	void Gamepad();
	void Keyboard();
};
