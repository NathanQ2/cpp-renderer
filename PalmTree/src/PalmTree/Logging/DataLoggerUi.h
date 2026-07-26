#pragma once
#include "DataLogger.h"
#include "PalmTree/Layer.h"

namespace PalmTree {
    class DataLoggerUI : public Layer {
    public:
        DataLoggerUI() : m_Logger(DataLogger::Get()) {}
        
        void OnImGuiRender() override;
    private:
        RootDataLogger* m_Logger;
        
        void DisplayLoggable(Loggable loggable);
    };
}
