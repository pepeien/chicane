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
        virtual void onActivation() { return; };

    public:
        void activate();

        template<class T = Pawn>
        const T* getPawn() const
        {
            return dynamic_cast<T*>(m_pawn);
        }

        Subscription<Pawn*>* watchAttachment(
            std::function<void (Pawn*)> inNext,
            std::function<void (const std::string&)> inError = nullptr,
            std::function<void ()> inComplete = nullptr
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
        void onMouseMotionEvent(const SDL_MouseMotionEvent& inEvent);
        void onMouseButtonEvent(const SDL_MouseButtonEvent& inEvent);
        void onKeyboardButtonEvent(const SDL_KeyboardEvent& inEvent);
        void onControllerMotionEvent(const SDL_GamepadAxisEvent& inEvent);
        void onControllerButtonEvent(const SDL_GamepadButtonEvent& inEvent);

        void clearEvents();

    private:
        // Pawn
        Pawn*                                                                    m_pawn;
        std::unique_ptr<Observable<Pawn*>>                                       m_pawnObservable;

        // Mouse Events
        std::vector<std::function<void(const SDL_MouseMotionEvent&)>>            m_mouseMotionEvents;
        std::unordered_map<std::uint8_t, std::vector<std::function<void(bool)>>> m_mouseButtonEvents;

        // Keyboard Events
        std::unordered_map<SDL_Scancode, std::vector<std::function<void(bool)>>> m_keyboardButtonEvents;

        // Controller Events
        std::vector<std::function<void(const SDL_GamepadAxisEvent&)>>            m_controllerMotionEvents;
        std::unordered_map<std::uint8_t, std::vector<std::function<void(bool)>>> m_controllerButtonEvents;
    };
}