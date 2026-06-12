#pragma once

#include <format>

#include "Event.h"

namespace PalmTree {
    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float mouseX, float mouseY) : m_MouseX(mouseX), m_MouseY(mouseY) {}

        EVENT_CLASS_IMPL_CATEGORY (EventCategoryInput
        |
        EventCategoryMouse
        )
        EVENT_CLASS_IMPL_TYPE (EventType::MouseMoved)

        std::string ToString() const override {
            return std::format("MouseMovedEvent: MousePosition=(%d, %d)", m_MouseX, m_MouseY);
        }

        float GetMouseX() const { return m_MouseX; }
        float GetMouseY() const { return m_MouseY; }

    private:
        float m_MouseX, m_MouseY;
    };

    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float scrollX, float scrollY) : m_ScrollX(scrollX), m_ScrollY(scrollY) {}

        EVENT_CLASS_IMPL_CATEGORY (EventCategoryInput
        |
        EventCategoryMouse
        )
        EVENT_CLASS_IMPL_TYPE (EventType::MouseScrolled)

        std::string ToString() const override {
            return std::format("MouseScrolledEvent: Scroll=(%d, %d)", m_ScrollX, m_ScrollY);
        }

    private:
        float m_ScrollX, m_ScrollY;
    };

    class MouseButtonEvent : public Event {
    public:
        int GetMouseButton() const { return m_Button; }


        EVENT_CLASS_IMPL_CATEGORY (EventCategoryInput
        |
        EventCategoryMouse
        |
        EventCategoryMouseButton
        )

    protected:
        MouseButtonEvent(int button) : m_Button(button) {}

        int m_Button;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        std::string ToString() const override {
            return std::format("MouseButtonPressedEvent: Button=%d", m_Button);
        }

        EVENT_CLASS_IMPL_TYPE (EventType::MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        std::string ToString() const override {
            return std::format("MouseButtonReleasedEvent: Button=%d", m_Button);
        }

        EVENT_CLASS_IMPL_TYPE (EventType::MouseButtonReleased)
    };
}
