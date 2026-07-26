#include "DataLoggerUi.h"

#include "imgui.h"

namespace PalmTree {
    void DataLoggerUI::OnImGuiRender() {
        ImGui::ShowDemoWindow();
        
        ImGui::Begin("DataLog Viewer");
        
        ImGuiTableFlags tableFlags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
        
        if (ImGui::BeginTable("Data", 2, tableFlags)) {
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();
            
            for (auto& [path, value] : m_Logger->m_Data) {
                if (value.size() == 0) continue;
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", path.c_str());
                
                ImGui::TableNextColumn();
                DisplayLoggable(value[value.size() - 1].Data);
            }
            
            ImGui::EndTable();
        }
        
        ImGui::End();
    }

    void DataLoggerUI::DisplayLoggable(Loggable loggable) {
        std::visit([loggable](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, float>) 
                ImGui::Text("%f", std::get<float>(loggable));
            else if constexpr (std::is_same_v<T, double>)
                ImGui::Text("%f", std::get<double>(loggable));
            else if constexpr (std::is_same_v<T, uint64_t>)
                ImGui::Text("%llu", std::get<uint64_t>(loggable));
            else if constexpr (std::is_same_v<T, bool>)
                ImGui::Text("%b", std::get<bool>(loggable));
            else if constexpr (std::is_same_v<T, std::string>)
                ImGui::Text("%s", std::get<std::string>(loggable).c_str());
            else if constexpr (std::is_same_v<T, glm::vec3>) {
                glm::vec3 vec = std::get<glm::vec3>(loggable);
                ImGui::Text("(%f, %f, %f)", vec.x, vec.y, vec.z);
            }
        }, loggable);
    }
}
