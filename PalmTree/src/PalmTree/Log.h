#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>

namespace PalmTree {
    class Log {
    public:
        static void init();

        static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
        static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_clientLogger; }
    private:
        static std::shared_ptr<spdlog::logger> s_coreLogger;
        static std::shared_ptr<spdlog::logger> s_clientLogger;
    };
}

#define PT_CORE_TRACE(...) ::PalmTree::Log::getCoreLogger()->trace(__VA_ARGS__)
#define PT_CORE_INFO(...)  ::PalmTree::Log::getCoreLogger()->info(__VA_ARGS__)
#define PT_CORE_WARN(...)  ::PalmTree::Log::getCoreLogger()->warn(__VA_ARGS__)
#define PT_CORE_ERROR(...) ::PalmTree::Log::getCoreLogger()->error(__VA_ARGS__)

#define PT_TRACE(...) ::PalmTree::Log::getClientLogger()->trace(__VA_ARGS__)
#define PT_INFO(...)  ::PalmTree::Log::getClientLogger()->info(__VA_ARGS__)
#define PT_WARN(...)  ::PalmTree::Log::getClientLogger()->warn(__VA_ARGS__)
#define PT_ERROR(...) ::PalmTree::Log::getClientLogger()->error(__VA_ARGS__)
