#pragma once


#include <spdlog/spdlog.h>

namespace PalmTree {
    class Log {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

#define PT_CORE_TRACE(...) ::PalmTree::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PT_CORE_INFO(...)  ::PalmTree::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PT_CORE_WARN(...)  ::PalmTree::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PT_CORE_ERROR(...) ::PalmTree::Log::GetCoreLogger()->error(__VA_ARGS__)

#define PT_TRACE(...) ::PalmTree::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PT_INFO(...)  ::PalmTree::Log::GetClientLogger()->info(__VA_ARGS__)
#define PT_WARN(...)  ::PalmTree::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PT_ERROR(...) ::PalmTree::Log::GetClientLogger()->error(__VA_ARGS__)

#ifdef PT_ENABLE_ASSERTS
    #include <cassert>
    #define PT_CORE_ASSERT(x, ...) if(!(x)) { PT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(false); }
    #define PT_ASSERT(x, ...) if(!(x)) { PT_ERROR("Assertion Failed: {0}", __VA_ARGS__); assert(false); }
#else
    #define PT_CORE_ASSERT(x, ...)
    #define PT_ASSERT(x, ...)
#endif
