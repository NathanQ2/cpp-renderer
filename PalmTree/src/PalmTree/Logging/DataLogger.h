#pragma once

#include <string>
#include <filesystem>
#include <variant>

#include <glm/glm.hpp>

#include "Log.h"

namespace PalmTree {
    using Loggable = std::variant<float, double, uint64_t, bool, std::string, glm::vec3>;
    using LogPath = std::filesystem::path;
    using LogTimestamp = std::chrono::steady_clock::time_point;
    
    enum class LoggableDataType : size_t {
        Float = Loggable(0.0f).index(),
        Double = Loggable(0.0).index(),
        Uint64 = Loggable((uint64_t)0).index(),
        Bool = Loggable(false).index(),
        String = Loggable("").index(),
        Vec3 = Loggable(glm::vec3()).index()
    };
    
    struct LogDataEntry {
        LogTimestamp Timestamp;
        Loggable Data;
    };
    
    class DataLoggerUI;
    
    class RootDataLogger {
        friend DataLoggerUI;
    public:
        void Record(const LogPath& path, const Loggable& value, LogTimestamp timestamp);
        void Record(const LogPath& path, const Loggable& value);

        std::optional<Loggable> Get(const LogPath& path, LogTimestamp timestamp);
        std::optional<Loggable> Get(const LogPath& path);
        
        void SetTimestamp(LogTimestamp timestamp) { m_Timestamp = timestamp; }
        
        LogTimestamp GetTimestamp() const { return m_Timestamp; }
        
        std::optional<LoggableDataType> GetDataType(const LogPath& path);
        
        void SetEnabled(bool enabled) { m_Enabled = enabled; }
        bool IsEnabled() const { return m_Enabled; }
    private:
        LogTimestamp m_Timestamp;
        
        std::unordered_map<LogPath, std::vector<LogDataEntry>> m_Data;
        
        bool m_Enabled = false;
    };
    
    class DataLogger {
        friend RootDataLogger;
    public:
        static void Init();
        static RootDataLogger* Get();
        
        static LogTimestamp GetTimestamp() { return s_Root->GetTimestamp(); }
        static void SetTimestamp(LogTimestamp timestamp) { return s_Root->SetTimestamp(timestamp); }
        
        DataLogger(LogPath path) : m_RootPath(path) {}
        
        DataLogger GetChild(LogPath subpath);
        
        void Record(const LogPath& path, const Loggable& value) const { s_Root->Record(m_RootPath / path, value); }
        
        template<typename T>
        std::optional<T> Get(const LogPath& path) {
            std::optional<Loggable> result = s_Root->Get(path);
            if (result && std::holds_alternative<T>(result.value())) 
                return std::get<T>(result.value());
            
            return std::nullopt;
        }
    private:
        static RootDataLogger* s_Root;
        
        const std::filesystem::path m_RootPath;
    };
}
