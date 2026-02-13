// This file includes dinput.h while disabling warnings
// in order to cut down on unnecessary backgound noise
// during compilation

#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#pragma warning(push, 0)
#include <dinput.h>
#pragma warning(pop)
#else
#include "Win32Compat.h"
#endif
