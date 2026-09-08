#pragma once

#include <glm/glm.hpp>

namespace PalmTree {
    class Input {
    public:
        static void Init();

        static bool IsKeyDown(int keycode) { return s_Instance->IsKeyDownImpl(keycode); }
        static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

        static bool IsMouseButtonDown(int button) { return s_Instance->IsMouseButtonDownImpl(button); }

        static glm::vec2 GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
        static float GetMousePositionX() { return s_Instance->GetMousePositionXImpl(); }
        static float GetMousePositionY() { return s_Instance->GetMousePositionYImpl(); }

        static void SetCursorEnabled(bool enabled) { s_Instance->SetCursorEnabledImpl(enabled); }

        virtual ~Input() = default;
    protected:
        virtual bool IsKeyDownImpl(int keycode) = 0;
        virtual bool IsKeyPressedImpl(int keycode) = 0;

        virtual bool IsMouseButtonDownImpl(int button) = 0;

        virtual glm::vec2 GetMousePositionImpl() = 0;
        virtual float GetMousePositionXImpl() = 0;
        virtual float GetMousePositionYImpl() = 0;
        virtual void SetCursorEnabledImpl(bool enabled) = 0;
    private:
        static Input* s_Instance;
    };
}
