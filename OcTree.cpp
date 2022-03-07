#include "OcTree.h"
#include <Windows.h>

//#include <DirectXCollision.h>
//#include <DirectXMath.h>

COcTree::COcTree()
{
	ZeroMemory(Children, sizeof(Children));
}

/*
COcTree::COcTree(XMVECTOR bounds1, XMVECTOR bounds2, ModelObject* pObjects, int objectCount)
{
	ZeroMemory(Children, sizeof(Children));

	Bounds.X1 = bounds1.m128_f32[0];
	Bounds.Y1 = bounds1.m128_f32[1];
	Bounds.Z1 = bounds1.m128_f32[2];

	Bounds.X2 = bounds2.m128_f32[0];
	Bounds.Y2 = bounds2.m128_f32[1];
	Bounds.Z2 = bounds2.m128_f32[2];
}
*/

COcTree::~COcTree()
{
	for (int i = 0; i < 8; i++)
	{
		if (Children[i] != NULL)
		{
			delete Children[i];
			Children[i] = NULL;
		}
	}
}

int COcTree::HitTest(XMVECTOR eye, XMVECTOR dir)
{
	//XMVECTOR veye;

	XMVECTOR v0;
	XMVECTOR v1;
	XMVECTOR v2;
	float dist = 0.0f;

	//bool hit = TriangleTests::Intersects(eye, dir, v0, v1, v2, dist);

	return -1;
}
