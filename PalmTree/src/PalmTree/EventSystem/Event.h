#pragma once

#include <bitset>
#include <string_view>
#include <functional>

#include "Event.h"
#include "../Core.h"

namespace PalmTree {
    enum class EventType {
        None = 0,
        WindowClosed, WindowResized,
        MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased,
        KeyPressed, KeyReleased
    };
    
    enum EventCategory {
        EventCategoryNone        = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput       = BIT(1),
        EventCategoryMouse       = BIT(2),
        EventCategoryMouseButton = BIT(3),
        EventCategoryKeyboard    = BIT(4),
    };
    
    
    class Event {
        friend class EventDispatcher;
    public:
        virtual ~Event() = default;
        
        virtual EventType getType() const = 0;
        virtual EventCategory getEventCategory() const = 0;
        // TODO: Strip from release builds
        virtual std::string_view getName() const = 0;
        virtual std::string toString() const { return std::string(getName()); }
        
        bool isInCategory(EventCategory category) const {
            return getEventCategory() & category;
        }
    private:
        bool m_handled = false;
    };
    
#define EVENT_CLASS_IMPL_CATEGORY(category) EventCategory getEventCategory() const override { return static_cast<EventCategory>(category); }
    
#define EVENT_CLASS_IMPL_TYPE(type) static EventType getStaticType() { return type;} \
    EventType getType() const override { return getStaticType(); } \
    std::string_view getName() const override { return #type; }
    
    class EventDispatcher {
        template<typename T>
        using EventFn = std::function<bool(T&)>;
        
    public:
        EventDispatcher(Event& event) : m_event(event) {}
        
        template<typename T>
        bool dispatch(EventFn<T> func) {
            if (m_event.getType() == T::getStaticType()) {
                m_event.m_handled = func(*(T*)(&m_event));
                
                return true;
            }
            
            return false;
        }
    private:
        Event& m_event;
    };
}
