#pragma once

#include "Base.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace spg {
	class SPG_API Log {
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define SPG_CORE_CRITICAL(...)	::spg::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define SPG_CORE_ERROR(...)		::spg::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SPG_CORE_WARN(...)		::spg::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SPG_CORE_INFO(...)		::spg::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SPG_CORE_TRACE(...)		::spg::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Client log macros
#define SPG_CRITICAL(...)		::spg::Log::GetClientLogger()->critical(__VA_ARGS__)
#define SPG_ERROR(...)			::spg::Log::GetClientLogger()->error(__VA_ARGS__)
#define SPG_WARN(...)			::spg::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SPG_INFO(...)			::spg::Log::GetClientLogger()->info(__VA_ARGS__)
#define SPG_TRACE(...)			::spg::Log::GetClientLogger()->trace(__VA_ARGS__)