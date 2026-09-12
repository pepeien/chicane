#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

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
            Insert, /**< insert on PC, help on some Mac keyboards (but does send
                       code 73, not 117) */

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
            Application,    /**< windows contextual menu, compose */
            Power,          /**< The USB document says this is a status flag,
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
            Stop,  /**< AC Stop */
            Again, /**< AC Redo/Repeat */
            Undo,  /**< AC Undo */
            Cut,   /**< AC Cut */
            Copy,  /**< AC Copy */
            Paste, /**< AC Paste */
            Find,  /**< AC Find */
            Mute,
            Volumeup,
            Volumedown,

            KpComma = 133,
            KpEqualsas400,

            International1, /**< used on Asian keyboards, see footnotes in USB
                               doc */
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

            AcSearch,    /**< AC Search */
            AcHome,      /**< AC Home */
            AcBack,      /**< AC Back */
            AcForward,   /**< AC Forward */
            AcStop,      /**< AC Stop */
            AcRefresh,   /**< AC Refresh */
            AcBookmarks, /**< AC Bookmarks */

            Softleft,  /**< Usually situated below the display on phones and
                        * used as a multi-function feature key for selecting
                        * a software defined function shown on the bottom left
                        * of the display.
                        */
            Softright, /**< Usually situated below the display on phones and
                        * used as a multi-function feature key for selecting
                        * a software defined function shown on the bottom right
                        * of the display.
                        */
            Call,      /**< Used for accepting phone calls. */
            Endcall    /**< Used for rejecting phone calls. */
        };

        inline String toString(KeyboardButton inValue)
        {
            switch (inValue)
            {
            case KeyboardButton::A:
                return "A";

            case KeyboardButton::B:
                return "B";

            case KeyboardButton::C:
                return "C";

            case KeyboardButton::D:
                return "D";

            case KeyboardButton::E:
                return "E";

            case KeyboardButton::F:
                return "F";

            case KeyboardButton::G:
                return "G";

            case KeyboardButton::H:
                return "H";

            case KeyboardButton::I:
                return "I";

            case KeyboardButton::J:
                return "J";

            case KeyboardButton::K:
                return "K";

            case KeyboardButton::L:
                return "L";

            case KeyboardButton::M:
                return "M";

            case KeyboardButton::N:
                return "N";

            case KeyboardButton::O:
                return "O";

            case KeyboardButton::P:
                return "P";

            case KeyboardButton::Q:
                return "Q";

            case KeyboardButton::R:
                return "R";

            case KeyboardButton::S:
                return "S";

            case KeyboardButton::T:
                return "T";

            case KeyboardButton::U:
                return "U";

            case KeyboardButton::V:
                return "V";

            case KeyboardButton::W:
                return "W";

            case KeyboardButton::X:
                return "X";

            case KeyboardButton::Y:
                return "Y";

            case KeyboardButton::Z:
                return "Z";

            case KeyboardButton::Number1:
                return "Number1";

            case KeyboardButton::Number2:
                return "Number2";

            case KeyboardButton::Number3:
                return "Number3";

            case KeyboardButton::Number4:
                return "Number4";

            case KeyboardButton::Number5:
                return "Number5";

            case KeyboardButton::Number6:
                return "Number6";

            case KeyboardButton::Number7:
                return "Number7";

            case KeyboardButton::Number8:
                return "Number8";

            case KeyboardButton::Number9:
                return "Number9";

            case KeyboardButton::Number0:
                return "Number0";

            case KeyboardButton::Return:
                return "Return";

            case KeyboardButton::Escape:
                return "Escape";

            case KeyboardButton::Backspace:
                return "Backspace";

            case KeyboardButton::Tab:
                return "Tab";

            case KeyboardButton::Space:
                return "Space";

            case KeyboardButton::Minus:
                return "Minus";

            case KeyboardButton::Equals:
                return "Equals";

            case KeyboardButton::Leftbracket:
                return "Leftbracket";

            case KeyboardButton::Rightbracket:
                return "Rightbracket";

            case KeyboardButton::Backslash:
                return "Backslash";

            case KeyboardButton::Nonushash:
                return "Nonushash";

            case KeyboardButton::Semicolon:
                return "Semicolon";

            case KeyboardButton::Apostrophe:
                return "Apostrophe";

            case KeyboardButton::Grave:
                return "Grave";

            case KeyboardButton::Comma:
                return "Comma";

            case KeyboardButton::Period:
                return "Period";

            case KeyboardButton::Slash:
                return "Slash";

            case KeyboardButton::Capslock:
                return "Capslock";

            case KeyboardButton::F1:
                return "F1";

            case KeyboardButton::F2:
                return "F2";

            case KeyboardButton::F3:
                return "F3";

            case KeyboardButton::F4:
                return "F4";

            case KeyboardButton::F5:
                return "F5";

            case KeyboardButton::F6:
                return "F6";

            case KeyboardButton::F7:
                return "F7";

            case KeyboardButton::F8:
                return "F8";

            case KeyboardButton::F9:
                return "F9";

            case KeyboardButton::F10:
                return "F10";

            case KeyboardButton::F11:
                return "F11";

            case KeyboardButton::F12:
                return "F12";

            case KeyboardButton::Printscreen:
                return "Printscreen";

            case KeyboardButton::Scrolllock:
                return "Scrolllock";

            case KeyboardButton::Pause:
                return "Pause";

            case KeyboardButton::Insert:
                return "Insert";

            case KeyboardButton::Home:
                return "Home";

            case KeyboardButton::Pageup:
                return "Pageup";

            case KeyboardButton::Delete:
                return "Delete";

            case KeyboardButton::End:
                return "End";

            case KeyboardButton::Pagedown:
                return "Pagedown";

            case KeyboardButton::Right:
                return "Right";

            case KeyboardButton::Left:
                return "Left";

            case KeyboardButton::Down:
                return "Down";

            case KeyboardButton::Up:
                return "Up";

            case KeyboardButton::Numlockclear:
                return "Numlockclear";

            case KeyboardButton::KpDivide:
                return "KpDivide";

            case KeyboardButton::KpMultiply:
                return "KpMultiply";

            case KeyboardButton::KpMinus:
                return "KpMinus";

            case KeyboardButton::KpPlus:
                return "KpPlus";

            case KeyboardButton::KpEnter:
                return "KpEnter";

            case KeyboardButton::Kp1:
                return "Kp1";

            case KeyboardButton::Kp2:
                return "Kp2";

            case KeyboardButton::Kp3:
                return "Kp3";

            case KeyboardButton::Kp4:
                return "Kp4";

            case KeyboardButton::Kp5:
                return "Kp5";

            case KeyboardButton::Kp6:
                return "Kp6";

            case KeyboardButton::Kp7:
                return "Kp7";

            case KeyboardButton::Kp8:
                return "Kp8";

            case KeyboardButton::Kp9:
                return "Kp9";

            case KeyboardButton::Kp0:
                return "Kp0";

            case KeyboardButton::KpPeriod:
                return "KpPeriod";

            case KeyboardButton::Nonusbackslash:
                return "Nonusbackslash";

            case KeyboardButton::Application:
                return "Application";

            case KeyboardButton::Power:
                return "Power";

            case KeyboardButton::KpEquals:
                return "KpEquals";

            case KeyboardButton::F13:
                return "F13";

            case KeyboardButton::F14:
                return "F14";

            case KeyboardButton::F15:
                return "F15";

            case KeyboardButton::F16:
                return "F16";

            case KeyboardButton::F17:
                return "F17";

            case KeyboardButton::F18:
                return "F18";

            case KeyboardButton::F19:
                return "F19";

            case KeyboardButton::F20:
                return "F20";

            case KeyboardButton::F21:
                return "F21";

            case KeyboardButton::F22:
                return "F22";

            case KeyboardButton::F23:
                return "F23";

            case KeyboardButton::F24:
                return "F24";

            case KeyboardButton::Execute:
                return "Execute";

            case KeyboardButton::Help:
                return "Help";

            case KeyboardButton::Menu:
                return "Menu";

            case KeyboardButton::Select:
                return "Select";

            case KeyboardButton::Stop:
                return "Stop";

            case KeyboardButton::Again:
                return "Again";

            case KeyboardButton::Undo:
                return "Undo";

            case KeyboardButton::Cut:
                return "Cut";

            case KeyboardButton::Copy:
                return "Copy";

            case KeyboardButton::Paste:
                return "Paste";

            case KeyboardButton::Find:
                return "Find";

            case KeyboardButton::Mute:
                return "Mute";

            case KeyboardButton::Volumeup:
                return "Volumeup";

            case KeyboardButton::Volumedown:
                return "Volumedown";

            case KeyboardButton::KpComma:
                return "KpComma";

            case KeyboardButton::KpEqualsas400:
                return "KpEqualsas400";

            case KeyboardButton::International1:
                return "International1";

            case KeyboardButton::International2:
                return "International2";

            case KeyboardButton::International3:
                return "International3";

            case KeyboardButton::International4:
                return "International4";

            case KeyboardButton::International5:
                return "International5";

            case KeyboardButton::International6:
                return "International6";

            case KeyboardButton::International7:
                return "International7";

            case KeyboardButton::International8:
                return "International8";

            case KeyboardButton::International9:
                return "International9";

            case KeyboardButton::Lang1:
                return "Lang1";

            case KeyboardButton::Lang2:
                return "Lang2";

            case KeyboardButton::Lang3:
                return "Lang3";

            case KeyboardButton::Lang4:
                return "Lang4";

            case KeyboardButton::Lang5:
                return "Lang5";

            case KeyboardButton::Lang6:
                return "Lang6";

            case KeyboardButton::Lang7:
                return "Lang7";

            case KeyboardButton::Lang8:
                return "Lang8";

            case KeyboardButton::Lang9:
                return "Lang9";

            case KeyboardButton::Alterase:
                return "Alterase";

            case KeyboardButton::Sysreq:
                return "Sysreq";

            case KeyboardButton::Cancel:
                return "Cancel";

            case KeyboardButton::Clear:
                return "Clear";

            case KeyboardButton::Prior:
                return "Prior";

            case KeyboardButton::Return2:
                return "Return2";

            case KeyboardButton::Separator:
                return "Separator";

            case KeyboardButton::Out:
                return "Out";

            case KeyboardButton::Oper:
                return "Oper";

            case KeyboardButton::Clearagain:
                return "Clearagain";

            case KeyboardButton::Crsel:
                return "Crsel";

            case KeyboardButton::Exsel:
                return "Exsel";

            case KeyboardButton::Kp00:
                return "Kp00";

            case KeyboardButton::Kp000:
                return "Kp000";

            case KeyboardButton::Thousandsseparator:
                return "Thousandsseparator";

            case KeyboardButton::Decimalseparator:
                return "Decimalseparator";

            case KeyboardButton::Currencyunit:
                return "Currencyunit";

            case KeyboardButton::Currencysubunit:
                return "Currencysubunit";

            case KeyboardButton::KpLeftparen:
                return "KpLeftparen";

            case KeyboardButton::KpRightparen:
                return "KpRightparen";

            case KeyboardButton::KpLeftbrace:
                return "KpLeftbrace";

            case KeyboardButton::KpRightbrace:
                return "KpRightbrace";

            case KeyboardButton::KpTab:
                return "KpTab";

            case KeyboardButton::KpBackspace:
                return "KpBackspace";

            case KeyboardButton::KpA:
                return "KpA";

            case KeyboardButton::KpB:
                return "KpB";

            case KeyboardButton::KpC:
                return "KpC";

            case KeyboardButton::KpD:
                return "KpD";

            case KeyboardButton::KpE:
                return "KpE";

            case KeyboardButton::KpF:
                return "KpF";

            case KeyboardButton::KpXor:
                return "KpXor";

            case KeyboardButton::KpPower:
                return "KpPower";

            case KeyboardButton::KpPercent:
                return "KpPercent";

            case KeyboardButton::KpLess:
                return "KpLess";

            case KeyboardButton::KpGreater:
                return "KpGreater";

            case KeyboardButton::KpAmpersand:
                return "KpAmpersand";

            case KeyboardButton::KpDblampersand:
                return "KpDblampersand";

            case KeyboardButton::KpVerticalbar:
                return "KpVerticalbar";

            case KeyboardButton::KpDblverticalbar:
                return "KpDblverticalbar";

            case KeyboardButton::KpColon:
                return "KpColon";

            case KeyboardButton::KpHash:
                return "KpHash";

            case KeyboardButton::KpSpace:
                return "KpSpace";

            case KeyboardButton::KpAt:
                return "KpAt";

            case KeyboardButton::KpExclam:
                return "KpExclam";

            case KeyboardButton::KpMemstore:
                return "KpMemstore";

            case KeyboardButton::KpMemrecall:
                return "KpMemrecall";

            case KeyboardButton::KpMemclear:
                return "KpMemclear";

            case KeyboardButton::KpMemadd:
                return "KpMemadd";

            case KeyboardButton::KpMemsubtract:
                return "KpMemsubtract";

            case KeyboardButton::KpMemmultiply:
                return "KpMemmultiply";

            case KeyboardButton::KpMemdivide:
                return "KpMemdivide";

            case KeyboardButton::KpPlusminus:
                return "KpPlusminus";

            case KeyboardButton::KpClear:
                return "KpClear";

            case KeyboardButton::KpClearentry:
                return "KpClearentry";

            case KeyboardButton::KpBinary:
                return "KpBinary";

            case KeyboardButton::KpOctal:
                return "KpOctal";

            case KeyboardButton::KpDecimal:
                return "KpDecimal";

            case KeyboardButton::KpHexadecimal:
                return "KpHexadecimal";

            case KeyboardButton::LCtrl:
                return "LCtrl";

            case KeyboardButton::LShift:
                return "LShift";

            case KeyboardButton::LAlt:
                return "LAlt";

            case KeyboardButton::LGui:
                return "LGui";

            case KeyboardButton::RCtrl:
                return "RCtrl";

            case KeyboardButton::RShift:
                return "RShift";

            case KeyboardButton::RAlt:
                return "RAlt";

            case KeyboardButton::RGui:
                return "RGui";

            case KeyboardButton::Mode:
                return "Mode";

            case KeyboardButton::Sleep:
                return "Sleep";

            case KeyboardButton::Wake:
                return "Wake";

            case KeyboardButton::ChannelIncrement:
                return "ChannelIncrement";

            case KeyboardButton::ChannelDecrement:
                return "ChannelDecrement";

            case KeyboardButton::MediaPlay:
                return "MediaPlay";

            case KeyboardButton::MediaPause:
                return "MediaPause";

            case KeyboardButton::MediaRecord:
                return "MediaRecord";

            case KeyboardButton::MediaFastForward:
                return "MediaFastForward";

            case KeyboardButton::MediaRewind:
                return "MediaRewind";

            case KeyboardButton::MediaNextTrack:
                return "MediaNextTrack";

            case KeyboardButton::MediaPreviousTrack:
                return "MediaPreviousTrack";

            case KeyboardButton::MediaStop:
                return "MediaStop";

            case KeyboardButton::MediaEject:
                return "MediaEject";

            case KeyboardButton::MediaPlayPause:
                return "MediaPlayPause";

            case KeyboardButton::MediaSelect:
                return "MediaSelect";

            case KeyboardButton::AcNew:
                return "AcNew";

            case KeyboardButton::AcOpen:
                return "AcOpen";

            case KeyboardButton::AcClose:
                return "AcClose";

            case KeyboardButton::AcExit:
                return "AcExit";

            case KeyboardButton::AcSave:
                return "AcSave";

            case KeyboardButton::AcPrint:
                return "AcPrint";

            case KeyboardButton::AcProperties:
                return "AcProperties";

            case KeyboardButton::AcSearch:
                return "AcSearch";

            case KeyboardButton::AcHome:
                return "AcHome";

            case KeyboardButton::AcBack:
                return "AcBack";

            case KeyboardButton::AcForward:
                return "AcForward";

            case KeyboardButton::AcStop:
                return "AcStop";

            case KeyboardButton::AcRefresh:
                return "AcRefresh";

            case KeyboardButton::AcBookmarks:
                return "AcBookmarks";

            case KeyboardButton::Softleft:
                return "Softleft";

            case KeyboardButton::Softright:
                return "Softright";

            case KeyboardButton::Call:
                return "Call";

            case KeyboardButton::Endcall:
                return "Endcall";

            default:
                return "";
            }
        }
    }
}