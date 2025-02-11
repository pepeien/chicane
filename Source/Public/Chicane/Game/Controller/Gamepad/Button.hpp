#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Controller
    {
        namespace Gamepad
        {
            enum class Button : std::uint8_t
            {
                South                = SDL_GAMEPAD_BUTTON_SOUTH,
                East                 = SDL_GAMEPAD_BUTTON_EAST,
                West                 = SDL_GAMEPAD_BUTTON_WEST,
                North                = SDL_GAMEPAD_BUTTON_NORTH,
                Back                 = SDL_GAMEPAD_BUTTON_BACK,
                Guide                = SDL_GAMEPAD_BUTTON_GUIDE,
                Start                = SDL_GAMEPAD_BUTTON_START,
                LeftStick            = SDL_GAMEPAD_BUTTON_LEFT_STICK,
                RightStick           = SDL_GAMEPAD_BUTTON_RIGHT_STICK,
                LeftShoulder         = SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
                RightShoulder        = SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
                DPadUp               = SDL_GAMEPAD_BUTTON_DPAD_UP,
                DPadDown             = SDL_GAMEPAD_BUTTON_DPAD_DOWN,
                DPadLeft             = SDL_GAMEPAD_BUTTON_DPAD_LEFT,
                DPadRight            = SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
                RightPaddlePrimary   = SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1,
                LeftPaddlePrimary    = SDL_GAMEPAD_BUTTON_LEFT_PADDLE1,
                RightPaddleSecondary = SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2,
                LeftPaddleSecondary  = SDL_GAMEPAD_BUTTON_LEFT_PADDLE2,
                Touchpad             = SDL_GAMEPAD_BUTTON_TOUCHPAD,
                Misc1                = SDL_GAMEPAD_BUTTON_MISC1,
                Misc2                = SDL_GAMEPAD_BUTTON_MISC2,
                Misc3                = SDL_GAMEPAD_BUTTON_MISC3,
                Misc4                = SDL_GAMEPAD_BUTTON_MISC4,
                Misc5                = SDL_GAMEPAD_BUTTON_MISC5,
                Misc6                = SDL_GAMEPAD_BUTTON_MISC6
            };
        }
    }
}