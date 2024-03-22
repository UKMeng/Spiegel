#pragma once

#ifdef SPG_PLATFORM_WINDOWS
#if SPG_DYNAMIC_LINK
	#ifdef SPG_BUILD_DLL
		#define SPG_API __declspec(dllexport)
	#else
		#define SPG_API __declspec(dllimport)
	#endif
#else
	#define SPG_API
#endif
#else
	#error Spiegel ONLY support Windows
#endif

#ifdef SPG_DEBUG
	#define SPG_ENABLE_ASSERTS
#endif

#ifdef SPG_ENABLE_ASSERTS
	#define SPG_ASSERT(x, ...) { if(!x) { SPG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define SPG_CORE_ASSERT(x, ...) { if(!x) { SPG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define SPG_ASSERT(x, ...)
	#define SPG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define SPG_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)