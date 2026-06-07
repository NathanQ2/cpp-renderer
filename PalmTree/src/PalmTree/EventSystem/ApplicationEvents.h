#pragma once
#include <format>

#include "Event.h"

namespace PalmTree {
    class WindowResizedEvent : public Event {
    public:
        WindowResizedEvent(int width, int height) : m_width(width), m_height(height) {}
        
        EVENT_CLASS_IMPL_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_IMPL_TYPE(EventType::WindowResized)
        
        std::string toString() const override {
            return std::format("WindowResizedEvent: Width={}, Height={}", m_width, m_height);
        }
        
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
    private:
        unsigned int m_width, m_height;
    };
    
    class WindowClosedEvent : public Event {
    public:
        WindowClosedEvent() = default;
        
        EVENT_CLASS_IMPL_CATEGORY(EventCategoryApplication)
        EVENT_CLASS_IMPL_TYPE(EventType::WindowClosed)
    };
}
