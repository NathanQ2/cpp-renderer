#pragma once

#include <format>

#include "Event.h"

namespace PalmTree {
    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float mouseX, float mouseY) : m_mouseX(mouseX), m_mouseY(mouseY) {}
        
        EVENT_CLASS_IMPL_CATEGORY(EventCategoryInput | EventCategoryMouse)
        EVENT_CLASS_IMPL_TYPE(EventType::MouseMoved)
        
        std::string toString() const override {
            return std::format("MouseMovedEvent: MousePosition=(%d, %d)", m_mouseX, m_mouseY);
        }
        
        float getMouseX() const { return m_mouseX; }
        float getMouseY() const { return m_mouseY; }
    private:
        float m_mouseX, m_mouseY;
    };
    
    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float scrollX, float scrollY) : m_scrollX(scrollX), m_scrollY(scrollY) {}
        
        EVENT_CLASS_IMPL_CATEGORY(EventCategoryInput | EventCategoryMouse)
        EVENT_CLASS_IMPL_TYPE(EventType::MouseScrolled)
        
        std::string toString() const override {
            return std::format("MouseScrolledEvent: Scroll=(%d, %d)", m_scrollX, m_scrollY);
        }
    private:
        float m_scrollX, m_scrollY;
    };
    
    class MouseButtonEvent : public Event {
    public:
        int getMouseButton() const { return m_button; }
        
        
        EVENT_CLASS_IMPL_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)
    protected:
        MouseButtonEvent(int button) : m_button(button) {}
        
        int m_button;
    };
    
    class MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}
        
        std::string toString() const override {
            return std::format("MouseButtonPressedEvent: Button=%d", m_button);
        }
        
        EVENT_CLASS_IMPL_TYPE(EventType::MouseButtonPressed)
    };
    
    class MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}
        
        std::string toString() const override {
            return std::format("MouseButtonReleasedEvent: Button=%d", m_button);
        }
        
        EVENT_CLASS_IMPL_TYPE(EventType::MouseButtonReleased)
    };
}
