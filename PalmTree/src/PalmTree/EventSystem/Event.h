#pragma once

#include <bitset>

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
        EventCategoryNone = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryMouse = BIT(2),
        EventCategoryMouseButton = BIT(3),
        EventCategoryKeyboard = BIT(4),
    };


    class Event {
        friend class EventDispatcher;

    public:
        virtual ~Event() = default;

        virtual EventType GetType() const = 0;
        virtual EventCategory GetEventCategory() const = 0;
        // TODO: Strip from release builds
        virtual std::string_view GetName() const = 0;
        virtual std::string ToString() const { return std::string(GetName()); }

        bool IsInCategory(EventCategory category) const {
            return GetEventCategory() & category;
        }

    private:
        bool m_Handled = false;
    };

#define EVENT_CLASS_IMPL_CATEGORY(category) EventCategory GetEventCategory() const override { return static_cast<EventCategory>(category); }

#define EVENT_CLASS_IMPL_TYPE(type) static EventType GetStaticType() { return type; } \
    EventType GetType() const override { return GetStaticType(); } \
    std::string_view GetName() const override { return #type; }

    class EventDispatcher {
        template <typename T>
        using EventFn = std::function<bool(T&)>;

    public:
        EventDispatcher(Event& event) : m_Event(event) {}

        template <typename T>
        bool Dispatch(EventFn<T> func) {
            if (m_Event.GetType() == T::GetStaticType()) {
                m_Event.m_Handled = func(*(T*)(&m_Event));

                return true;
            }

            return false;
        }

    private:
        Event& m_Event;
    };
}
