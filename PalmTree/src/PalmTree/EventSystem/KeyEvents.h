#pragma once
#include <format>

#include "Event.h"

namespace PalmTree {
    class KeyEvent : public Event {
    public:
        EVENT_CLASS_IMPL_CATEGORY (EventCategoryInput
        |
        EventCategoryKeyboard
        )

        int GetKeyCode() const { return m_KeyCode; }

    protected:
        KeyEvent(int keyCode) : m_KeyCode(keyCode) {}

        int m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

        EVENT_CLASS_IMPL_TYPE (EventType::KeyPressed)

        std::string ToString() const override {
            return std::format("KeyPressedEvent: KeyCode={}, RepeatCount={}", m_KeyCode, m_RepeatCount);
        }

        int GetRepeatCount() const { return m_RepeatCount; }
        bool IsRepeating() const { return m_RepeatCount > 0; }

    private:
        int m_RepeatCount;
    };

    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

        EVENT_CLASS_IMPL_TYPE (EventType::KeyReleased)

        std::string ToString() const override {
            return std::format("KeyReleasedEvent: KeyCode={}", m_KeyCode);
        }
    };
}
