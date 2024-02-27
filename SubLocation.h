#pragma once

#include "DXText.h"

class CSubLocation
{
public:
	int ParentLocation;
	std::string Text;
	CDXText RealText;
	float Top;
	float Left;
	float Bottom;
	float Right;

	CSubLocation(int parent, std::string text) { Left = Top = Right = Bottom = 0.0f; ParentLocation = parent; Text = text; RealText.SetText((char*)text.c_str()); }
};
