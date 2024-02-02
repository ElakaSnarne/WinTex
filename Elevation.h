#pragma once

#include <Windows.h>

struct Elevation
{
	int Type;
	int Id;
	int Trigger;
	int X1;
	int Y1;
	int Z1;
	int X2;
	int Y2;
	int Z2;
	int StepHeight;
	int StepLength;
	int StepWidth;
	int NumberOfSteps;
	int HorizontalLength;
	int DirectionX;
	int DirectionZ;
};

class CElevation
{
public:
	CElevation(Elevation* pElevationData);
	~CElevation();

	BOOL IsPointInElevation(float x, float y, float z);
	float GetElevationFromXZPosition(float x, float z);
	float GetClosestElevation(float y);

	static float ElevationModifier;
	static float ElevationCheckModifier;

protected:
	float _x1;
	float _y1;
	float _z1;
	float _x2;
	float _y2;
	float _z2;
	float _stepHeight;
	float _stepLength;
	float _stepWidth;
	int _numberOfSteps;
	float _horizontalLength;
	float _xDirection;
	float _zDirection;

	float _x11;
	float _x12;
	float _x21;
	float _x22;
	float _z11;
	float _z12;
	float _z21;
	float _z22;

	float _xMin;
	float _xMax;
	float _yMin;
	float _yMax;
	float _zMin;
	float _zMax;
};
