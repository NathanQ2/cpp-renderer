#pragma once

#include "PtDescriptors.h"
#include "EntityComponentSystem/EntityComponentSystem.h"
#include "PtModel.h"
#include "PtRenderer.h"
#include "PtWindow.h"

#include <memory>
#include <vector>

namespace PalmTree {
    class PtApplication {
    public:
        PtApplication();
        ~PtApplication() = default;

        PtApplication(const PtApplication&) = delete;
        PtApplication& operator=(const PtApplication&) = delete;

        void run();

    private:
        void loadGameObjects();

        const int m_width = 800;
        const int m_height = 600;
        const std::string m_title = "PalmTree Window";

        PtWindow m_window = PtWindow(m_width, m_height, m_title);
        PtDevice m_device = PtDevice(m_window);
        PtRenderer m_renderer = PtRenderer(m_window, m_device);

        // NOTE: Must be declared after PtDevice
        std::unique_ptr<PtDescriptorPool> m_globalPool{};
       
        EntityComponentSystem m_ecs{};
    };
}
