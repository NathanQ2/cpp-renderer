#include "DataLoggerUI.h"

#include "PalmTree/Application.h"

#include <imgui.h>
#include <implot.h>
#include <ranges>

namespace PalmTree {
    void DataLoggerUI::OnUpdate(float dt) {
        m_ApplicationTime = m_Logger->GetTimestamp();
    }

    void DataLoggerUI::OnImGuiRender() {
        ImGui::ShowDemoWindow();

        ShowDataLogTableWindow();
        ShowPlotWindows();
    }

    void DataLoggerUI::ShowDataLogTableWindow() {
        ImGui::Begin("DataLog Table");
        
        ImGui::Checkbox("Enabled", &m_Logger->m_Enabled);
        
        ImGui::InputInt("Number of Plots", &m_NumPlotWindows, 1, 5);
        if (ImGui::Button("Reset Plots")) {
            for (auto& [id, data ] : m_PlotWindowData) {
                data = PlotWindowData{};
            }
        }

        ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH |
            ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody |
            ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_HighlightHoveredColumn;
        
        if (ImGui::BeginTable("Data", 2, tableFlags)) {
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            int row = 0;
            for (auto& [path, value] : m_Logger->m_Data) {
                if (value.size() == 0) continue;

                ImGui::PushID(row);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Selectable(path.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
                
                if (ImGui::BeginDragDropSource()) {
                    std::string pathStr = path;
                    ImGui::SetDragDropPayload("DATALOG_PATH_TYPE", pathStr.c_str(), sizeof(char) * pathStr.size());
                    ImGui::Text("%s", path.c_str());
                    ImGui::EndDragDropSource();
                }

                ImGui::TableNextColumn();
                DisplayLoggable(value[value.size() - 1].Data);
                ImGui::PopID();
                row++;
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }

    void DataLoggerUI::ShowPlotWindows() {
        for (int i = 0; i < m_NumPlotWindows; i++) {
            if (!m_PlotWindowData[i].IsOpen)
                continue;
            
            ImGui::PushID(i);
            std::string windowName = fmt::format("Plot {}", i);
            ImGui::Begin(windowName.c_str(), &m_PlotWindowData[i].IsOpen, ImGuiWindowFlags_MenuBar);
            
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("Plot Settings")) {
                    if (ImGui::MenuItem("Reset")) {
                        m_PlotWindowData[i] = PlotWindowData{};
                    }
                        
                    ImGui::MenuItem("AutoFit X", nullptr, &m_PlotWindowData[i].AutoFitX);
                    ImGui::DragFloat("History", &m_PlotWindowData[i].History, 0.1f, -1.0f, 60.0f, "%0.3fs");
                    ImGui::MenuItem("AutoFit Y", nullptr, &m_PlotWindowData[i].AutoFitY);
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenuBar();
            }
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImVec2 avail = ImGui::GetContentRegionAvail();
            if (ImPlot::BeginPlot(windowName.c_str(), avail)) {
                ImPlot::SetupAxes("Application Time (Seconds)", nullptr, ImPlotAxisFlags_None, ImPlotAxisFlags_None);
                if (m_PlotWindowData[i].AutoFitX) {
                    float curTime = TimestampToFloat(m_ApplicationTime);
                    float history = m_PlotWindowData[i].History;
                    float beginTime = history >= 0.0f ? glm::max(curTime - history, 0.0f) : 0.0f; 
                    ImPlot::SetupAxisLimits(ImAxis_X1, beginTime, curTime, ImGuiCond_Always);
                }
                
                if (m_PlotWindowData[i].AutoFitY && !m_PlotWindowData[i].Data.empty()) {
                    auto [min, max] = FindPlotMinMaxValue(m_PlotWindowData[i]);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, min, max, ImGuiCond_Always);
                }
                
                if (ImPlot::BeginDragDropTargetPlot()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DATALOG_PATH_TYPE")) {
                        PlotWindowData::LineData& data = m_PlotWindowData[i].Data.emplace_back(static_cast<const char*>(payload->Data));
                        
                        std::optional<LoggableDataType> type = m_Logger->GetDataType(data.Path);
                        PT_CORE_ASSERT(type.has_value(), "Attempting to plot invalid data");
                        data.DataType = type.value();
                        
                        if (data.DataType == LoggableDataType::Vec3) {
                            ImGui::OpenPopup("VectorSelectAxisPopup");
                        }
                        else {
                            RefreshLineData(data);
                        }
                    }
                
                    ImPlot::EndDragDropTarget();
                }
                
                ImGui::PopStyleVar();
                
                if (ImGui::BeginPopup("VectorSelectAxisPopup")) {
                    if (ImGui::MenuItem("X")) m_PlotWindowData[i].Data.back().VectorIndex = 0;
                    if (ImGui::MenuItem("Y")) m_PlotWindowData[i].Data.back().VectorIndex = 1;
                    if (ImGui::MenuItem("Z")) m_PlotWindowData[i].Data.back().VectorIndex = 2;
                    ImGui::EndPopup();
                }

                for (PlotWindowData::LineData& data : m_PlotWindowData[i].Data) {
                    const std::string& path = data.Path;
                    const std::vector<float>& x = data.X;
                    const std::vector<float>& y = data.Y;
                    
                    if (path.empty()) continue;
                    if (m_Logger->m_Data[path].size() != x.size()) {
                        data.Dirty = true;
                        RefreshLineData(data);
                    }
                    
                    PT_CORE_ASSERT(x.size() == y.size(), "Invalid plot data!");
                    switch (data.DataType) {
                        case LoggableDataType::Bool:
                            ImPlot::PlotDigital(path.data(), x.data(), y.data(), x.size());
                            
                            break;
                        default:
                            ImPlot::PlotLine(path.data(), x.data(), y.data(), x.size());
                            
                            break;
                    }
                }
            
                ImPlot::EndPlot();
            }
            else {
                ImGui::PopStyleVar();
            }
            
            ImGui::End();
            ImGui::PopID();
        }
    }

    void DataLoggerUI::DisplayLoggable(Loggable loggable) {
        std::visit(
            [loggable](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, float>)
                    ImGui::Text("%f", std::get<float>(loggable));
                else if constexpr (std::is_same_v<T, double>)
                    ImGui::Text("%f", std::get<double>(loggable));
                else if constexpr (std::is_same_v<T, uint64_t>)
                    ImGui::Text("%llu", std::get<uint64_t>(loggable));
                else if constexpr (std::is_same_v<T, bool>)
                    ImGui::Text("%s", std::get<bool>(loggable) ? "true" : "false");
                else if constexpr (std::is_same_v<T, std::string>)
                    ImGui::Text("%s", std::get<std::string>(loggable).c_str());
                else if constexpr (std::is_same_v<T, glm::vec3>) {
                    glm::vec3 vec = std::get<glm::vec3>(loggable);
                    ImGui::Text("(%f, %f, %f)", vec.x, vec.y, vec.z);
                }
            },
            loggable
        );
    }

    void DataLoggerUI::RefreshLineData(PlotWindowData::LineData& data) {
        PT_CORE_ASSERT(!data.Path.empty(), "Invalid data path");
        
        int size = m_Logger->m_Data[data.Path].size();
        std::vector<float>& x = data.X;
        std::vector<float>& y = data.Y;
        
        x.reserve(size);
        y.reserve(size);
            
        for (auto& [timestamp, value] : m_Logger->m_Data[data.Path]) {
            float time = TimestampToFloat(timestamp);
            
            // Check if this entry is already in the vector
            if (!x.empty() && x.back() > time) continue;
            
            x.emplace_back(time);
            float val = std::visit([&data](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                                
                if constexpr (std::is_same_v<T, float>) return static_cast<float>(arg);
                if constexpr (std::is_same_v<T, double>) return static_cast<float>(arg);
                if constexpr (std::is_same_v<T, uint64_t>) return static_cast<float>(arg);
                if constexpr (std::is_same_v<T, bool>) return static_cast<float>(static_cast<bool>(arg));
                if constexpr (std::is_same_v<T, std::string>) PT_CORE_ASSERT(false, "String data cannot be displayed in plot.");
                if constexpr (std::is_same_v<T, glm::vec3>) return static_cast<float>(arg[data.VectorIndex]);
                                
                PT_CORE_ASSERT(false, "Unsupported plot data type");
                return 0.0f;
            }, value);
            y.emplace_back(val);
        }
        
        data.Dirty = false;
    }

    float DataLoggerUI::TimestampToFloat(LogTimestamp timestamp) {
        return (float)std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - m_ApplicationStartTime).count() / 1000.0f;
    }

    std::pair<float, float> DataLoggerUI::FindPlotMinMaxValue(const PlotWindowData& plot) {
        float curMin = plot.Data[0].Y[0];
        float curMax = plot.Data[0].Y[0];
        
        for (const std::vector<float>& y : plot.Data | std::ranges::views::transform([](const PlotWindowData::LineData& data) { return data.Y; })) {
            float min = *std::ranges::min_element(y);
            float max = *std::ranges::max_element(y);
            
            if (curMin > min) curMin = min;
            if (curMax < max) curMax = max;
        }
        
        return std::pair(curMin, curMax);
    }
}
