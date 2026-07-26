#include "DataLogger.h"

#include <ranges>

#include "Log.h"

namespace PalmTree {
    RootDataLogger* DataLogger::s_Root = nullptr;

    void RootDataLogger::Record(const LogPath& path, const Loggable& value, LogTimestamp timestamp) {
        if (!m_Data[path].empty()) {
            PT_CORE_ASSERT(value.index() == m_Data[path][0].Data.index(), "Logged value data type must always remain the same");
        }
        
        m_Data[path].emplace_back(timestamp, value);
    }

    void RootDataLogger::Record(const LogPath& path, const Loggable& value) { Record(path, value, m_Timestamp); }

    std::optional<Loggable> RootDataLogger::Get(const LogPath& path, LogTimestamp timestamp) {
        for (auto& [t, data] : std::views::reverse(m_Data[path])) {
            if (t == timestamp) {
                return data;
            }
        }
        
        return std::nullopt;
    }

    std::optional<Loggable> RootDataLogger::Get(const LogPath& path) { return Get(path, m_Timestamp); }

    void DataLogger::Init() {
        PT_CORE_ASSERT(!s_Root, "DataLogger has already been initialized!");
        
        s_Root = new RootDataLogger();
    }

    RootDataLogger* DataLogger::Get() {
        return s_Root;
    }

    DataLogger DataLogger::GetChild(LogPath subpath) {
        return DataLogger(m_RootPath / subpath);
    }
}
