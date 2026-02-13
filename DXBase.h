#pragma once

#include "D3D11-NoWarn.h"
#include "D3DX11-NoWarn.h"
#include "Structs.h"
#include "Platform.h"

#ifdef PLATFORM_LINUX
// No ATL or DXPackedVector on Linux
#include "Win32Compat.h"
#else
#include <atlbase.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#endif

#ifdef PLATFORM_WINDOWS
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#endif

class CDXBase
{
public:
	CDXBase();
	~CDXBase();
};
