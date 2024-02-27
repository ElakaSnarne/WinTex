#include "Elevation.h"
#include <cstdlib>

float CElevation::ElevationModifier = 1.5f;
float CElevation::ElevationCheckModifier = 1.5f;

CElevation::CElevation(Elevation* pElevationData)
{
	_x1 = ((float)pElevationData->X1) / 65536.0f;
	_y1 = ((float)pElevationData->Y1) / 65536.0f;
	_z1 = ((float)pElevationData->Z1) / 65536.0f;
	_x2 = ((float)pElevationData->X2) / 65536.0f;
	_y2 = ((float)pElevationData->Y2) / 65536.0f;
	_z2 = ((float)pElevationData->Z2) / 65536.0f;

	_stepHeight = ((float)pElevationData->StepHeight) / 65536.0f;
	//float stepLength = ((float)pElevationData->StepLength) / 65536.0f;
	_stepWidth = ((float)pElevationData->StepWidth) / 65536.0f;

	_numberOfSteps = pElevationData->NumberOfSteps;
	_horizontalLength = ((float)pElevationData->HorizontalLength) / 65536.0f;
	_xDirection = ((float)pElevationData->DirectionX) / 65536.0f;
	_zDirection = ((float)pElevationData->DirectionZ) / 65536.0f;

	float halfStepWidthX = abs(_zDirection) * _stepWidth / 1.0f;
	float halfStepWidthZ = abs(_xDirection) * _stepWidth / 1.0f;

	_x11 = _x1 + halfStepWidthX;
	_x12 = _x1 - halfStepWidthX;
	_x21 = _x2 + halfStepWidthX;
	_x22 = _x2 - halfStepWidthX;
	_z11 = _z1 + halfStepWidthZ;
	_z12 = _z1 - halfStepWidthZ;
	_z21 = _z2 + halfStepWidthZ;
	_z22 = _z2 - halfStepWidthZ;

	_xMin = min(min(min(_x11, _x12), _x21), _x22);
	_xMax = max(max(max(_x11, _x12), _x21), _x22);
	_zMin = min(min(min(_z11, _z12), _z21), _z22);
	_zMax = max(max(max(_z11, _z12), _z21), _z22);
	_yMin = min(_y1, _y2);
	_yMax = max(_y1, _y2);
}

CElevation::~CElevation()
{
}

BOOL CElevation::IsPointInElevation(float x, float y, float z)
{
	return (x >= _xMin && x <= _xMax && z >= _zMin && z <= _zMax && (y + ElevationCheckModifier) >= _yMin && (y - ElevationCheckModifier) <= _yMax);
}

float CElevation::GetElevationFromXZPosition(float x, float z)
{
	// Find percentage position along elevation
	double p = (1 - abs(_zDirection * (_z21 - z) + _xDirection * (_x21 - x)) / _horizontalLength);
	double sp = 1.0 / _numberOfSteps;
	double t = p / sp;
	double t2 = (double)((int)t);
	double t3 = t - t2;

	if (t3 >= 0.2 && t3 < 0.5)
	{
		t2 += (t3 - 0.2) / 0.3;
	}
	else if (t3 >= 0.5)
	{
		t2 += 1.0;
	}

	double d = (_y1 + _stepHeight * t2) - ElevationModifier;

	return (float)d;
}

float CElevation::GetClosestElevation(float y)
{
	float dy1 = y - (_y1 - 1.0f);
	float dy2 = y - (_y2 - 1.0f);

	return (((dy1 * dy1) < (dy2 * dy2)) ? _y1 : _y2) - ElevationModifier;
}
