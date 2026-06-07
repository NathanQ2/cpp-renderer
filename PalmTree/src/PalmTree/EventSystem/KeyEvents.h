#pragma once
#include <format>

#include "Event.h"

namespace PalmTree {
    class KeyEvent : public Event {
    public:
        EVENT_CLASS_IMPL_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
        
        int getKeyCode() const { return m_keyCode; }
    protected:
        KeyEvent(int keyCode) : m_keyCode(keyCode) {}
        
        int m_keyCode;
    };
    
    class KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode), m_repeatCount(repeatCount) {}
        
        EVENT_CLASS_IMPL_TYPE(EventType::KeyPressed)
        
        std::string toString() const override {
            return std::format("KeyPressedEvent: KeyCode={}, RepeatCount={}", m_keyCode, m_repeatCount);
        }
        
        int getRepeatCount() const { return m_repeatCount; }
        bool isRepeating() const { return m_repeatCount > 0; }
    private:
        int m_repeatCount;
    };
    
    class KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}
        
        EVENT_CLASS_IMPL_TYPE(EventType::KeyReleased)
        
        std::string toString() const override {
            return std::format("KeyReleasedEvent: KeyCode={}", m_keyCode);
        }
    };
}
