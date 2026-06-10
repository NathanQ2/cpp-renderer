#pragma once
#include <format>

#include "Event.h"

namespace PalmTree {
    class WindowResizedEvent : public Event {
    public:
        WindowResizedEvent(int width, int height) : m_Width(width), m_Height(height) {}
        
        EVENT_CLASS_IMPL_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_IMPL_TYPE(EventType::WindowResized)
        
        std::string ToString() const override {
            return std::format("WindowResizedEvent: Width={}, Height={}", m_Width, m_Height);
        }
        
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
    private:
        unsigned int m_Width, m_Height;
    };
    
    class WindowClosedEvent : public Event {
    public:
        WindowClosedEvent() = default;
        
        EVENT_CLASS_IMPL_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_IMPL_TYPE(EventType::WindowClosed)
    };
}
