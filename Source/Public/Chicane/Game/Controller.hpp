#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core.hpp"

namespace Chicane
{
    class Pawn;

    class Controller
    {
    public:
        Controller();
        virtual ~Controller() = default;

    public:
        template<class T = Pawn>
        const T* getPawn() const
        {
            return dynamic_cast<T*>(m_pawn);
        }

        void watchAttachment(
            std::function<void (Pawn*)> inNextCallback,
            std::function<void (const std::string&)> inErrorCallback = nullptr,
            std::function<void ()> inCompleteCallback = nullptr
        );

        bool isAttached() const;
        void attachTo(Pawn* inPawn);
        void deattach();

        // Mouse Events
        void bindMouseMotionEvent(std::function<void(const SDL_MouseMotionEvent&)> inEvent);
        void bindMouseButtonEvent(std::uint8_t inButtonCode, std::function<void(bool)> inEvent);

        // Keyboard Events
        void bindKeyboardButtonEvent(SDL_Scancode inButtonCode, std::function<void(bool)> inEvent);

        // Controller Events
        void bindControllerMotionEvent(std::function<void(const SDL_GamepadAxisEvent&)> inEvent);
        void bindControllerButtonEvent(std::uint8_t inButtonCode, std::function<void(bool)> inEvent);

        void onEvent(const SDL_Event& inEvent);

    private:
        // Events
        void onMouseButtonEvent(const SDL_MouseButtonEvent& inEvent);
        void onKeyboardButtonEvent(const SDL_KeyboardEvent& inEvent);
        void onControllerButtonEvent(const SDL_GamepadButtonEvent& inEvent);

        void clearEvents();

    private:
        // Pawn
        Pawn* m_pawn;
        std::unique_ptr<Observable<Pawn*>> m_pawnObservable;

        // Mouse Events
        std::function<void(const SDL_MouseMotionEvent&)> m_mouseMotionEvent;
        std::unordered_map<std::uint8_t, std::function<void(bool)>> m_mouseButtonEvents;

        // Keyboard Events
        std::unordered_map<SDL_Scancode, std::function<void(bool)>> m_keyboardButtonEvents;

        // Controller Events
        std::function<void(const SDL_GamepadAxisEvent&)> m_controllerMotionEvent;
        std::unordered_map<std::uint8_t, std::function<void(bool)>> m_controllerButtonEvents;
    };
}