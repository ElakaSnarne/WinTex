#pragma once

#include <vector>

struct Fixed
{
	unsigned short Low;
	short High;
};

struct SubSprite
{
	Fixed XAdjust;
	Fixed YAdjust;
	int TextureIndex;
	int Flags;
	Fixed U1;
	Fixed V1;
	Fixed U2;
	Fixed V2;
};

struct FixedPoint
{
	Fixed X;
	Fixed Y;
	Fixed Z;
};

struct LocationSprite
{
	int TextureIndex;
	FixedPoint* Point;
	std::vector<SubSprite> Sprites;
};
