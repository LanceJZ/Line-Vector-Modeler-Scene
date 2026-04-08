#include "ThePlayer.h"

ThePlayer::ThePlayer()
{
	EM.AddLineModel(MirrorModel = new LineModel());
}

ThePlayer::~ThePlayer()
{
}

bool ThePlayer::Initialize()
{
	LineModel::Initialize();

	return false;
}

bool ThePlayer::BeginRun()
{
	LineModel::BeginRun();

	MirrorModel->Enabled = false;
	MirrorModel->Alpha = 175;
	MirrorModel->ModelColor = LIGHTGRAY;
	MirrorModel->HideCollision = true;

	return false;
}

void ThePlayer::Input()
{
	LineModel::Input();

	Keyboard();

	if (IsGamepadAvailable(0))
	{
		Gamepad();
	}
}

void ThePlayer::Update(float deltaTime)
{
	LineModel::Update(deltaTime);

	MirrorModel->Enabled = false;
}

void ThePlayer::FixedUpdate(float deltaTime)
{
}

void ThePlayer::Draw3D()
{
	LineModel::Draw3D();

}


void ThePlayer::Reset()
{
	Position = { 0, 0, 0 };
	Velocity = { 0, 0, 0 };
	Scale = 1.0f;
	ModelIndex = -1;
	LinePoints.clear();
	Enabled = false;
	MirrorModel->Enabled = false;
	std::vector<Vector3> empty;
	MirrorModel->SetModel(empty);
}

void ThePlayer::Spawn()
{
}

void ThePlayer::NewGame()
{
	
}

void ThePlayer::ClearModel()
{
	LinePoints.clear();
}

void ThePlayer::AddPoint(Vector3 point)
{
	LinePoints.push_back(point);
	if (LinePoints.size() > 2) CalculateRadius();
}

void ThePlayer::AddPointAt(Vector3 point, size_t index)
{
	if (LinePoints.size() == index) LinePoints.push_back(point);
	else LinePoints.insert(LinePoints.begin() + index + 1, point);
	CalculateRadius();
}

void ThePlayer::MovePoint(Vector3 point, size_t index)
{
	LinePoints[index] = point;
	CalculateRadius();
}

void ThePlayer::DeletePoint(size_t index)
{
	LinePoints.erase(LinePoints.begin() + index);
	CalculateRadius();
}


void ThePlayer::Mirror()
{
	std::vector<Vector3> Mirroredpoints;

	// Make sure Y does not go below 0.
	for (auto point : LinePoints)
	{
		if (point.y < 0) return;
	}

	// Duplicate the points and reverse them on the Y axis, end to start.
	int startPoint = LinePoints.size() - 1;

	for (int i = startPoint; i > -1; i--)
	{
		float y = LinePoints[i].y;
		y *= -1.0f;
		Mirroredpoints.push_back({ LinePoints[i].x, y, LinePoints[i].z });
	}

	MirrorModel->SetModel(Mirroredpoints);
	MirrorModel->Enabled = true;
}

void ThePlayer::ApplyMirror()
{
	MirrorModel->Enabled = false;

	std::vector<Vector3> Mirroredpoints;

	int startPoint = LinePoints.size() - 2;

	for (int i = startPoint; i > -1; i--)
	{
		float y = LinePoints[i].y;
		y *= -1.0f;
		Mirroredpoints.push_back({ LinePoints[i].x, y, LinePoints[i].z });
	}

	for (auto &point : Mirroredpoints)
	{
		LinePoints.push_back({ point.x, point.y, point.z });
	}
}

void ThePlayer::Flip()
{
	for (auto &point : LinePoints)
	{
		point.x *= -1.0f;
	}
}

void ThePlayer::Center()
{
	float right = 0.0f, left = 0.0f, top = 0.0f, bottom = 0.0f;

	for (auto &point : LinePoints)
	{
		if (point.x > right) right = point.x;
		if (point.x < left) left = point.x;
		if (point.y > top) top = point.y;
		if (point.y < bottom) bottom = point.y;
	}

	left *= -1.0f;
	top *= -1.0f;

	float moveX;
	float moveY;

	if (left > right)
	{
		moveX = left - right;
	}
	else
	{
		moveX = (right - left) * -1.0f;
	}

	if (top > bottom)
	{
		moveY = top - bottom;
	}
	else
	{
		moveY = (bottom - top) * -1.0f;
	}

	for (auto &point : LinePoints)
	{
		point.x += moveX * 0.5f;
		point.y += moveY * 0.5f;
	}
}

void ThePlayer::SetScale(float scale)
{
	for (auto &point : LinePoints)
	{
		point.x *= scale;
		point.y *= scale;
	}
}

void ThePlayer::Gamepad()
{
	//Button B is 6 for Shield //Button A is 7 for Fire //Button Y is 8 for Hyperspace
	//Button X is 5	//Left bumper is 9 //Right bumper is 11 for Shield //Left Trigger is 10
	//Right Trigger is 12 for Thrust //Dpad Up is 1 for	//Dpad Down is 3 for
	//Dpad Left is 4 for rotate left //Dpad Right is 2 for rotate right
	//Axis 1 is -1 for , 1 for  on left stick.
	//Axis 0 is -1 for Left, 1 for right on left stick.

	if (IsGamepadButtonDown(0, 12))
	{
	}
	else
	{
	}

	if (IsGamepadButtonDown(0, 4) || GetGamepadAxisMovement(0, 0) < -0.25f)
	{
	}
	else if (IsGamepadButtonDown(0, 2) || GetGamepadAxisMovement(0, 0) > 0.25f)
	{
	}

	if (IsGamepadButtonPressed(0, 7))
	{
	}

	if (IsGamepadButtonDown(0, 11) || IsGamepadButtonDown(0, 6))
	{
	}
	else
	{
	}
}

void ThePlayer::Keyboard()
{
}