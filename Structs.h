#ifndef STRUCTS_H_
#define STRUCTS_H_
#pragma once

#include <D3DX11.h>
#include <DirectXPackedVector.h>
#include <list>
#include <string>

using namespace DirectX::PackedVector;
using namespace DirectX;

struct TEXTURED_VERTEX_ORTHO
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
};

struct TEXTURED_VERTEX
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT2 object; // Use for visibility, then create a visibility buffer for the objects and lookup in the shader
};

struct COLOURED_VERTEX
{
	XMFLOAT4 position;
	XMFLOAT4 colour;
	XMFLOAT4 object;
};

struct COLOURED_VERTEX_ORTHO
{
	XMFLOAT4 position;
	XMFLOAT4 colour;
};

struct VOPBufferType
{
	XMMATRIX  view;
	XMMATRIX  ortho;
	XMMATRIX  projection;
};

struct WorldBufferType
{
	XMMATRIX  world;
};

struct FontBufferType
{
	XMVECTOR colour;
};

struct TexFontBufferType
{
	XMVECTOR colour1;
	XMVECTOR colour2;
	XMVECTOR colour3;
	XMVECTOR colour4;
};

struct Point
{
	float X;
	float Y;
	float Z;
};

struct DPoint
{
	double X;
	double Y;
	double Z;
};

struct Line
{
	DPoint P1;
	DPoint P2;
};

struct TPoint
{
	Point* Point;
	float U;
	float V;
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

struct VisibilityBufferType
{
	XMFLOAT4 visibility[3000];
};

struct TranslationBufferType
{
	XMFLOAT4 translation[256];
};

struct ObjectMap
{
	int id;
	int ObjectIndex;
	std::list<int> SubObjectIndices;
};

enum ActionType
{
	None = 0x00000000,
	Look = 0x00000001,
	Move = 0x00000002,
	Get = 0x00000004,
	OnOff = 0x00000008,
	Talk = 0x00000010,
	Open = 0x00000020,
	Use = 0x00000040,
	Terminate = 0x00008000,
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

enum AnimationStatus
{
	NotStarted = 0,
	Running = 1,
	OnHold = 2,
	Completed = 4
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
