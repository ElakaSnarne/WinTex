// This file includes D3DX11.h while disabling warnings
// in order to cut down on unnecessary backgound noise
// during compilation

#pragma once

#include "Platform.h"

#ifdef PLATFORM_LINUX
#include "Win32Compat.h"
#else
#pragma warning(push,0)
#include <D3DX11.h>
#pragma warning(pop)
#endif
