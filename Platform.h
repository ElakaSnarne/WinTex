#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif !defined(PLATFORM_LINUX)
    #define PLATFORM_LINUX
#endif
