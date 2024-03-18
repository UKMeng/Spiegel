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

#ifdef SPG_ENABLE_ASSERTS
	#define SPG_ASSERT(x, ...) { if(!x) { SPG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define SPG_CORE_ASSERT(x, ...) { if(!x) { SPG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define SPG_ASSERT(x, ...)
	#define SPG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)