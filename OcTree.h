#ifndef OCTREE_H_
#define OCTREE_H_

#pragma once

//#include <D3DX11.h>

//#include <Windows.h>
//#include <D3D11.h>
//#include "Structs.h"
//#include <D3D11.h>
#include <DirectXMath.h>
#include "TexModel.h"

//using namespace DirectX;

class COcTree
{
public:
	//COcTree(XMVECTOR bounds1, XMVECTOR bounds2, ModelObject* pObjects, int objectCount);
	COcTree();
	virtual ~COcTree();

	int HitTest(DirectX::XMVECTOR eye, DirectX::XMVECTOR dir);

protected:
	COcTree* Children[8];

	//Box Bounds;
};

#endif
