#pragma once

#include <memory>

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
// TODO£º Implement a better assert
	#define SPG_ASSERT(x, ...) { if(!(x)) { SPG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define SPG_CORE_ASSERT(x, ...) { if(!(x)) { SPG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define SPG_ASSERT(x, ...)
	#define SPG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define SPG_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace spg {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}