#pragma once
#include "DataLogger.h"
#include "PalmTree/Layer.h"

namespace PalmTree {
    class DataLoggerUI : public Layer {
    public:
        DataLoggerUI(LogTimestamp applicationStartTime) : m_ApplicationStartTime(applicationStartTime),
                                                          m_Logger(DataLogger::Get()) {}

        void OnUpdate(float dt) override;
        void OnImGuiRender() override;
    private:
        struct PlotWindowData {
            struct LineData {
                std::string Path;
                LoggableDataType DataType;
                int VectorIndex = 0;
                std::vector<float> X;
                std::vector<float> Y;
                bool Dirty = true;
            };

            std::vector<LineData> Data;
            float History = -1.0f;
            bool AutoFitX = true;
            bool AutoFitY = true;
            bool IsOpen = true;
        };

        RootDataLogger* m_Logger;

        int m_NumPlotWindows = 0;
        std::unordered_map<int, PlotWindowData> m_PlotWindowData;

        LogTimestamp m_ApplicationStartTime;
        LogTimestamp m_ApplicationTime;

        void ShowDataLogTableWindow();
        void ShowPlotWindows();

        void DisplayLoggable(Loggable loggable);

        void RefreshLineData(PlotWindowData::LineData& data);

        float TimestampToFloat(LogTimestamp timestamp);

        static std::pair<float, float> FindPlotMinMaxValue(const PlotWindowData& plot);
    };
}
