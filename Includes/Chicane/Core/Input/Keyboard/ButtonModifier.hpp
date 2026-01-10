#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Input
    {
        // Extracted from `SDL3/SDL_Keymod`
        enum class KeyboardButtonModifier
        {
            None       = 0x0000u, /**< no modifier is applicable. */
            LeftShift  = 0x0001u, /**< the left Shift key is down. */
            RightShift = 0x0002u, /**< the right Shift key is down. */
            Leve5      = 0x0004u, /**< the Level 5 Shift key is down. */
            LeftCtrl   = 0x0040u, /**< the left Ctrl (Control) key is down. */
            RightCtrl  = 0x0080u, /**< the right Ctrl (Control) key is down. */
            LeftAlt    = 0x0100u, /**< the left Alt key is down. */
            RightAlt   = 0x0200u, /**< the right Alt key is down. */
            LeftGui  = 0x0400u, /**< the left GUI key (often the Windows key) is
                                   down. */
            RightGui = 0x0800u, /**< the right GUI key (often the Windows key)
                                   is down. */
            Num  = 0x1000u, /**< the Num Lock key (may be located on an extended
                               keypad) is down. */
            Caps = 0x2000u, /**< the Caps Lock key is down. */
            Mode = 0x4000u, /**< the !AltGr key is down. */
            Scroll = 0x8000u, /**< the Scroll Lock key is down. */
            Ctrl   = (LeftCtrl | RightCtrl),   /**< Any Ctrl key is down. */
            Shift  = (LeftShift | RightShift), /**< Any Shift key is down. */
            Alt    = (LeftAlt | RightAlt),     /**< Any Alt key is down. */
            Gui    = (LeftGui | RightGui),     /**< Any GUI key is down. */
        };
    }
}