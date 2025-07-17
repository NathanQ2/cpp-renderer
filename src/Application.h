#pragma once

#include "Pipeline.h"
#include "Window.h"

namespace PalmTree {
    class Application {
    public:
        Application();
        ~Application();

        void Run();
    private:
        const int m_Width = 1280;
        const int m_Height = 720;
        const std::string m_Title = "Test Window";
    
        Window m_Window = Window(m_Width, m_Height, m_Title);
        PalmTreeDevice m_Device= PalmTreeDevice(m_Window);
        Pipeline m_Pipeline = Pipeline(
            m_Device,
            "../shaders/simpleShader.vert.spv",
            "../shaders/simpleShader.frag.spv",
            Pipeline::DefaultPipelineConfig(m_Width, m_Height)
        );
    };
}
