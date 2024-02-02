#ifndef STRUCTS_H_
#define STRUCTS_H_
#pragma once

#include "D3DX11-NoWarn.h"
#include <DirectXPackedVector.h>
#include <list>
#include <string>
#include "Point.h"
#include "Enums.h"

using namespace DirectX::PackedVector;
using namespace DirectX;

struct Line
{
	DPoint P1;
	DPoint P2;
};

struct TLPoint
{
	Point* Point;
	float U;
	float V;

	int ObjectIndex;
	int SubObjectIndex;
	int SubObjectId;
};

struct PointUV
{
	float x;
	float y;
	float u;
	float v;
};

struct Triangle
{
	int ObjectId;
	int SubObjectId;
	int Flags;

	TLPoint P1;
	TLPoint P2;
	TLPoint P3;
};

struct Box
{
	float X1;
	float Y1;
	float Z1;
	float X2;
	float Y2;
	float Z2;
};

struct ModelSubObject
{
	int ModelIndex;
	int SubObjectIndex;
	int Flags;
	int Texture;
	int ID;
	int VerticeOffset;
	int PointCount;
	Box BoundingBox;
	BOOL Active;

	TLPoint* Points;
	Triangle* Triangles;
};

struct ModelObject
{
	int Index;
	int SubObjectCount;
	Box BoundingBox;
	ModelSubObject* SubObjects;
};

struct SpritePosInfo
{
	Point Position;
	BOOL Visible;
};

struct Buffer
{
	LPBYTE pData;
	int Size;
	int Frame;
};

struct Size
{
	float Width;
	float Height;
};

struct Rect
{
	float Top;
	float Left;
	float Bottom;
	float Right;
};

struct ListBoxItem
{
	int Id = 0;
	std::wstring Text = L"";
	int StartVertex = 0;
	int VerticeCount = 0;
	BOOL MouseOver = FALSE;
};

struct InventoryItem
{
	int Id;
	float ImageX1;
	float ImageY1;
	float ImageX2;
	float ImageY2;
	float NameX1;
	float NameY1;
	float NameX2;
	float NameY2;
};

struct Animation
{
	int Type;					// 1,2,3,4,14,15,16 (4 can have sub-type 1,2,3,4,5,6,7,8)
	AnimationStatus Status;
	DWORD FrameDuration;
	DWORD ConstantFrameDuration;
	ULONGLONG FrameTime;
	LPBYTE AnimDataPointer;
	LPBYTE AnimDataPointerInit;
	LPBYTE AnimDataPointerEnd;
	int ParentAnim;				// Used by 4.7, resume parent when this completes
	int ObjectId;
	int Parameter;
	int FrameCounter;
};

struct FrameData
{
	LPBYTE VideoPointer;
	int VideoSize;
	LPBYTE PalettePointer;
	int PaletteSize;
	LPBYTE AudioPointer;
	int AudiSize;

	int FrameNumber;
};

struct ControlCoordinates
{
	BYTE KeyCode;
	BYTE Flags;
	int MinY;
	int MaxY;
	int MinX;
	int MaxX;
	int Action;
};

struct ControlTable
{
	int ImageIndex;
	int X;
	int Y;
};

#endif
