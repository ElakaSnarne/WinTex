#pragma once

#include "Structs.h"

typedef struct IntersectionInfo
{
	DPoint LineSegmentP1;
	DPoint LineSegmentP2;
	DPoint ProjectionPoint;
	double ProjectionLength;
};
