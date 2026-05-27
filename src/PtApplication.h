#pragma once

#include "PtWindow.h"
#include "PtModel.h"
#include "PtGameObject.h"
#include "PtRenderer.h"
#include "PtDescriptors.h"

#include <memory>
#include <vector>

namespace PalmTree {
    class PtApplication {
    public:
        PtApplication();
        ~PtApplication() = default;

        PtApplication(const PtApplication&) = delete;
        PtApplication& operator=(const PtApplication&) = delete;

        void Run();
    private:
        void LoadGameObjects();
        
        const int m_Width = 800;
        const int m_Height = 600;
        const std::string m_Title = "PalmTree Window";
    
        PtWindow m_Window = PtWindow(m_Width, m_Height, m_Title);
        PtDevice m_Device = PtDevice(m_Window);
        PtRenderer m_Renderer = PtRenderer(m_Window, m_Device);

        // NOTE: Must be declared after PtDevice
        std::unique_ptr<PtDescriptorPool> m_GlobalPool{};
        PtGameObject::Map m_GameObjects;
    };
}
