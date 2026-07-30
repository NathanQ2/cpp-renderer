#pragma once

#include <string_view>

#include "EventSystem/Event.h"
#include "Renderer/FrameInfo.h"

namespace PalmTree {
    class Layer {
    public:
#ifdef PT_DEBUG
        explicit Layer(const std::string& name = "Layer") : m_Name(name) {}
#else
        explicit Layer(const std::string& = "") {}
#endif
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnStart() {}
        virtual void OnEnd() {}

        virtual void OnUpdate(float dt) {}
        virtual void OnRender(float dt) {}

        virtual bool OnEvent(Event& event) { return false; }

        virtual void OnImGuiRender() {}

        void SetEnabled(bool enable) { m_Enabled = enable; }
        bool IsEnabled() const { return m_Enabled; }

#ifdef PT_DEBUG
        const std::string& GetName() const { return m_Name; }
#endif
    private:
#ifdef PT_DEBUG
        std::string m_Name;
#endif

        bool m_Enabled = true;
    };
}
