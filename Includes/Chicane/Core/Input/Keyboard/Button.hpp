#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Input
    {
        // Extracted from `SDL3/SDL_Scancode`
        enum class KeyboardButton
        {
            A = 4,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,

            Number1,
            Number2,
            Number3,
            Number4,
            Number5,
            Number6,
            Number7,
            Number8,
            Number9,
            Number0,

            Return,
            Escape,
            Backspace,
            Tab,
            Space,

            Minus,
            Equals,
            Leftbracket,
            Rightbracket,
            Backslash, /**< Located at the lower left of the return
                        *   key on ISO keyboards and at the right end
                        *   of the QWERTY row on ANSI keyboards.
                        *   Produces REVERSE SOLIDUS (backslash) and
                        *   VERTICAL LINE in a US layout, REVERSE
                        *   SOLIDUS and VERTICAL LINE in a UK Mac
                        *   layout, NUMBER SIGN and TILDE in a UK
                        *   Windows layout, DOLLAR SIGN and POUND SIGN
                        *   in a Swiss German layout, NUMBER SIGN and
                        *   APOSTROPHE in a German layout, GRAVE
                        *   ACCENT and POUND SIGN in a French Mac
                        *   layout, and ASTERISK and MICRO SIGN in a
                        *   French Windows layout.
                        */
            Nonushash, /**< ISO USB keyboards actually use this code
                        *   instead of 49 for the same key, but all
                        *   OSes I've seen treat the two codes
                        *   identically. So, as an implementor, unless
                        *   your keyboard generates both of those
                        *   codes and your OS treats them differently,
                        *   you should generate SDL_SCANCODE_BACKSLASH
                        *   instead of this code. As a user, you
                        *   should not rely on this code because SDL
                        *   will never generate it with most (all?)
                        *   keyboards.
                        */
            Semicolon,
            Apostrophe,
            Grave, /**< Located in the top left corner (on both ANSI
                    *   and ISO keyboards). Produces GRAVE ACCENT and
                    *   TILDE in a US Windows layout and in US and UK
                    *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                    *   and NOT SIGN in a UK Windows layout, SECTION
                    *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                    *   layouts on ISO keyboards, SECTION SIGN and
                    *   DEGREE SIGN in a Swiss German layout (Mac:
                    *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                    *   DEGREE SIGN in a German layout (Mac: only on
                    *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                    *   French Windows layout, COMMERCIAL AT and
                    *   NUMBER SIGN in a French Mac layout on ISO
                    *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                    *   SIGN in a Swiss German, German, or French Mac
                    *   layout on ANSI keyboards.
                    */
            Comma,
            Period,
            Slash,

            Capslock,

            F1,
            F2,
            F3,
            F4,
            F5,
            F6,
            F7,
            F8,
            F9,
            F10,
            F11,
            F12,

            Printscreen,
            Scrolllock,
            Pause,
            Insert, /**< insert on PC, help on some Mac keyboards (but does send code 73, not 117) */

            Home,
            Pageup,
            Delete,
            End,
            Pagedown,
            Right,
            Left,
            Down,
            Up,

            Numlockclear, /**< num lock on PC, clear on Mac keyboards */

            KpDivide,
            KpMultiply,
            KpMinus,
            KpPlus,
            KpEnter,
            Kp1,
            Kp2,
            Kp3,
            Kp4,
            Kp5,
            Kp6,
            Kp7,
            Kp8,
            Kp9,
            Kp0,
            KpPeriod,

            Nonusbackslash, /**< This is the additional key that ISO
                            *   keyboards have over ANSI ones,
                            *   located between left shift and Y.
                            *   Produces GRAVE ACCENT and TILDE in a
                            *   US or UK Mac layout, REVERSE SOLIDUS
                            *   (backslash) and VERTICAL LINE in a
                            *   US or UK Windows layout, and
                            *   LESS-THAN SIGN and GREATER-THAN SIGN
                            *   in a Swiss German, German, or French
                            *   layout.
                            */
            Application, /**< windows contextual menu, compose */
            Power, /**< The USB document says this is a status flag,
                    *   not a physical key - but some Mac keyboards
                    *   do have a power key.
                    */

            KpEquals,
            F13,
            F14,
            F15,
            F16,
            F17,
            F18,
            F19,
            F20,
            F21,
            F22,
            F23,
            F24,
            Execute,
            Help, /**< AL Integrated Help Center */
            Menu, /**< Menu (show menu) */
            Select,
            Stop, /**< AC Stop */
            Again, /**< AC Redo/Repeat */
            Undo, /**< AC Undo */
            Cut, /**< AC Cut */
            Copy, /**< AC Copy */
            Paste, /**< AC Paste */
            Find, /**< AC Find */
            Mute,
            Volumeup,
            Volumedown,

            KpComma = 133,
            KpEqualsas400,

            International1, /**< used on Asian keyboards, see footnotes in USB doc */
            International2,
            International3, /**< Yen */
            International4,
            International5,
            International6,
            International7,
            International8,
            International9,
            Lang1, /**< Hangul/English toggle */
            Lang2, /**< Hanja conversion */
            Lang3, /**< Katakana */
            Lang4, /**< Hiragana */
            Lang5, /**< Zenkaku/Hankaku */
            Lang6, /**< reserved */
            Lang7, /**< reserved */
            Lang8, /**< reserved */
            Lang9, /**< reserved */

            Alterase, /**< Erase-Eaze */
            Sysreq,
            Cancel, /**< AC Cancel */
            Clear,
            Prior,
            Return2,
            Separator,
            Out,
            Oper,
            Clearagain,
            Crsel,
            Exsel,

            Kp00 = 176,
            Kp000,
            Thousandsseparator,
            Decimalseparator,
            Currencyunit,
            Currencysubunit,
            KpLeftparen,
            KpRightparen,
            KpLeftbrace,
            KpRightbrace,
            KpTab,
            KpBackspace,
            KpA,
            KpB,
            KpC,
            KpD,
            KpE,
            KpF,
            KpXor,
            KpPower,
            KpPercent,
            KpLess,
            KpGreater,
            KpAmpersand,
            KpDblampersand,
            KpVerticalbar,
            KpDblverticalbar,
            KpColon,
            KpHash,
            KpSpace,
            KpAt,
            KpExclam,
            KpMemstore,
            KpMemrecall,
            KpMemclear,
            KpMemadd,
            KpMemsubtract,
            KpMemmultiply,
            KpMemdivide,
            KpPlusminus,
            KpClear,
            KpClearentry,
            KpBinary,
            KpOctal,
            KpDecimal,
            KpHexadecimal,

            LCtrl = 224,
            LShift,
            LAlt, /**< alt, option */
            LGui, /**< windows, command (apple), meta */
            RCtrl,
            RShift,
            RAlt, /**< alt gr, option */
            RGui, /**< windows, command (apple), meta */

            Mode = 257, /**< I'm not sure if this is really not covered
                        *   by any of the above, but since there's a
                        *   special SDL_KMOD_MODE for it I'm adding it here
                        */

            Sleep, /**< Sleep */
            Wake,  /**< Wake */

            ChannelIncrement, /**< Channel Increment */
            ChannelDecrement, /**< Channel Decrement */

            MediaPlay,          /**< Play */
            MediaPause,         /**< Pause */
            MediaRecord,        /**< Record */
            MediaFastForward,   /**< Fast Forward */
            MediaRewind,        /**< Rewind */
            MediaNextTrack,     /**< Next Track */
            MediaPreviousTrack, /**< Previous Track */
            MediaStop,          /**< Stop */
            MediaEject,         /**< Eject */
            MediaPlayPause,     /**< Play / Pause */
            MediaSelect,        /* Media Select */

            AcNew,        /**< AC New */
            AcOpen,       /**< AC Open */
            AcClose,      /**< AC Close */
            AcExit,       /**< AC Exit */
            AcSave,       /**< AC Save */
            AcPrint,      /**< AC Print */
            AcProperties, /**< AC Properties */

            AcSearch,     /**< AC Search */
            AcHome,       /**< AC Home */
            AcBack,       /**< AC Back */
            AcForward,    /**< AC Forward */
            AcStop,       /**< AC Stop */
            AcRefresh,    /**< AC Refresh */
            AcBookmarks,  /**< AC Bookmarks */

            Softleft, /**< Usually situated below the display on phones and
                    * used as a multi-function feature key for selecting
                    * a software defined function shown on the bottom left
                    * of the display.
                    */
            Softright, /**< Usually situated below the display on phones and
                        * used as a multi-function feature key for selecting
                        * a software defined function shown on the bottom right
                        * of the display.
                        */
            Call, /**< Used for accepting phone calls. */
            Endcall /**< Used for rejecting phone calls. */
        };
    }
}