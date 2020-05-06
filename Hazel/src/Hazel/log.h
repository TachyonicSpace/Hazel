#pragma once

#include "Core.h"
#include "spdlog\spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog\fmt\ostr.h"

namespace Hazel {

	class HAZEL_API log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//core log macros
#define HZ_CORE_TRACE(...) ::Hazel::log::GetCoreLogger()->trace(__VA_ARGS__)
#define HZ_CORE_INFO(...)  ::Hazel::log::GetCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_WARN(...)  ::Hazel::log::GetCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_ERROR(...) ::Hazel::log::GetCoreLogger()->error(__VA_ARGS__)
#define HZ_CORE_FATAL(...) ::Hazel::log::GetCoreLogger()->fatal(__VA_ARGS__)

//client log macros
#define HZ_CLIENT_TRACE(...) ::Hazel::log::GetClientLogger()->trace(__VA_ARGS__)
#define HZ_CLIENT_INFO(...)  ::Hazel::log::GetClientLogger()->info(__VA_ARGS__)
#define HZ_CLIENT_WARN(...)  ::Hazel::log::GetClientLogger()->warn(__VA_ARGS__)
#define HZ_CLIENT_ERROR(...) ::Hazel::log::GetClientLogger()->error(__VA_ARGS__)
#define HZ_CLIENT_FATAL(...) ::Hazel::log::GetClientLogger()->fatal(__VA_ARGS__)