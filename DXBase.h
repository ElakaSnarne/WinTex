#pragma once

#include "D3D11-NoWarn.h"
#include "D3DX11-NoWarn.h"
#include "Structs.h"
#include <atlbase.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

class CDXBase
{
public:
	CDXBase();
	~CDXBase();
};
