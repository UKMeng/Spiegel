#pragma once

#ifdef SPG_PLATFORM_WINDOWS
	#ifdef SPG_BUILD_DLL
		#define SPG_API __declspec(dllexport)
	#else
		#define SPG_API __declspec(dllimport)
	#endif
#else
	#error Spiegel ONLY support Windows
#endif

#define BIT(x) (1 << x)