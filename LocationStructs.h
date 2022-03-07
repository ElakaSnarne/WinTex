#pragma once

struct LocationObject
{
	float BoundingBoxX1;
	float BoundingBoxX2;
	float BoundingBoxY1;
	float BoundingBoxY2;
	float BoundingBoxZ1;
	float BoundingBoxZ2;
	int SubObjectIndex;
	int SubObjectCount;
};

struct LocationSubObject
{
	float BoundingBoxX1;
	float BoundingBoxX2;
	float BoundingBoxY1;
	float BoundingBoxY2;
	float BoundingBoxZ1;
	float BoundingBoxZ2;

	int ID;
	int Texture;
	int TriangleIndex;
	int TriangleCount;

	int VertexIndex;
	int VertexCount;
};

struct LocationTriangle
{
	int P1;
	int P2;
	int P3;
};
