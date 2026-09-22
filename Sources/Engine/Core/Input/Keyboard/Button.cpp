#include "Chicane/Core/Input/Keyboard/Button.hpp"

namespace Chicane
{
    String toString(Input::KeyboardButton inValue)
    {
        switch (inValue)
        {
        case Input::KeyboardButton::A:
            return "A";

        case Input::KeyboardButton::B:
            return "B";

        case Input::KeyboardButton::C:
            return "C";

        case Input::KeyboardButton::D:
            return "D";

        case Input::KeyboardButton::E:
            return "E";

        case Input::KeyboardButton::F:
            return "F";

        case Input::KeyboardButton::G:
            return "G";

        case Input::KeyboardButton::H:
            return "H";

        case Input::KeyboardButton::I:
            return "I";

        case Input::KeyboardButton::J:
            return "J";

        case Input::KeyboardButton::K:
            return "K";

        case Input::KeyboardButton::L:
            return "L";

        case Input::KeyboardButton::M:
            return "M";

        case Input::KeyboardButton::N:
            return "N";

        case Input::KeyboardButton::O:
            return "O";

        case Input::KeyboardButton::P:
            return "P";

        case Input::KeyboardButton::Q:
            return "Q";

        case Input::KeyboardButton::R:
            return "R";

        case Input::KeyboardButton::S:
            return "S";

        case Input::KeyboardButton::T:
            return "T";

        case Input::KeyboardButton::U:
            return "U";

        case Input::KeyboardButton::V:
            return "V";

        case Input::KeyboardButton::W:
            return "W";

        case Input::KeyboardButton::X:
            return "X";

        case Input::KeyboardButton::Y:
            return "Y";

        case Input::KeyboardButton::Z:
            return "Z";

        case Input::KeyboardButton::Number1:
            return "Number1";

        case Input::KeyboardButton::Number2:
            return "Number2";

        case Input::KeyboardButton::Number3:
            return "Number3";

        case Input::KeyboardButton::Number4:
            return "Number4";

        case Input::KeyboardButton::Number5:
            return "Number5";

        case Input::KeyboardButton::Number6:
            return "Number6";

        case Input::KeyboardButton::Number7:
            return "Number7";

        case Input::KeyboardButton::Number8:
            return "Number8";

        case Input::KeyboardButton::Number9:
            return "Number9";

        case Input::KeyboardButton::Number0:
            return "Number0";

        case Input::KeyboardButton::Return:
            return "Return";

        case Input::KeyboardButton::Escape:
            return "Escape";

        case Input::KeyboardButton::Backspace:
            return "Backspace";

        case Input::KeyboardButton::Tab:
            return "Tab";

        case Input::KeyboardButton::Space:
            return "Space";

        case Input::KeyboardButton::Minus:
            return "Minus";

        case Input::KeyboardButton::Equals:
            return "Equals";

        case Input::KeyboardButton::Leftbracket:
            return "Leftbracket";

        case Input::KeyboardButton::Rightbracket:
            return "Rightbracket";

        case Input::KeyboardButton::Backslash:
            return "Backslash";

        case Input::KeyboardButton::Nonushash:
            return "Nonushash";

        case Input::KeyboardButton::Semicolon:
            return "Semicolon";

        case Input::KeyboardButton::Apostrophe:
            return "Apostrophe";

        case Input::KeyboardButton::Grave:
            return "Grave";

        case Input::KeyboardButton::Comma:
            return "Comma";

        case Input::KeyboardButton::Period:
            return "Period";

        case Input::KeyboardButton::Slash:
            return "Slash";

        case Input::KeyboardButton::Capslock:
            return "Capslock";

        case Input::KeyboardButton::F1:
            return "F1";

        case Input::KeyboardButton::F2:
            return "F2";

        case Input::KeyboardButton::F3:
            return "F3";

        case Input::KeyboardButton::F4:
            return "F4";

        case Input::KeyboardButton::F5:
            return "F5";

        case Input::KeyboardButton::F6:
            return "F6";

        case Input::KeyboardButton::F7:
            return "F7";

        case Input::KeyboardButton::F8:
            return "F8";

        case Input::KeyboardButton::F9:
            return "F9";

        case Input::KeyboardButton::F10:
            return "F10";

        case Input::KeyboardButton::F11:
            return "F11";

        case Input::KeyboardButton::F12:
            return "F12";

        case Input::KeyboardButton::Printscreen:
            return "Printscreen";

        case Input::KeyboardButton::Scrolllock:
            return "Scrolllock";

        case Input::KeyboardButton::Pause:
            return "Pause";

        case Input::KeyboardButton::Insert:
            return "Insert";

        case Input::KeyboardButton::Home:
            return "Home";

        case Input::KeyboardButton::Pageup:
            return "Pageup";

        case Input::KeyboardButton::Delete:
            return "Delete";

        case Input::KeyboardButton::End:
            return "End";

        case Input::KeyboardButton::Pagedown:
            return "Pagedown";

        case Input::KeyboardButton::Right:
            return "Right";

        case Input::KeyboardButton::Left:
            return "Left";

        case Input::KeyboardButton::Down:
            return "Down";

        case Input::KeyboardButton::Up:
            return "Up";

        case Input::KeyboardButton::Numlockclear:
            return "Numlockclear";

        case Input::KeyboardButton::KpDivide:
            return "KpDivide";

        case Input::KeyboardButton::KpMultiply:
            return "KpMultiply";

        case Input::KeyboardButton::KpMinus:
            return "KpMinus";

        case Input::KeyboardButton::KpPlus:
            return "KpPlus";

        case Input::KeyboardButton::KpEnter:
            return "KpEnter";

        case Input::KeyboardButton::Kp1:
            return "Kp1";

        case Input::KeyboardButton::Kp2:
            return "Kp2";

        case Input::KeyboardButton::Kp3:
            return "Kp3";

        case Input::KeyboardButton::Kp4:
            return "Kp4";

        case Input::KeyboardButton::Kp5:
            return "Kp5";

        case Input::KeyboardButton::Kp6:
            return "Kp6";

        case Input::KeyboardButton::Kp7:
            return "Kp7";

        case Input::KeyboardButton::Kp8:
            return "Kp8";

        case Input::KeyboardButton::Kp9:
            return "Kp9";

        case Input::KeyboardButton::Kp0:
            return "Kp0";

        case Input::KeyboardButton::KpPeriod:
            return "KpPeriod";

        case Input::KeyboardButton::Nonusbackslash:
            return "Nonusbackslash";

        case Input::KeyboardButton::Application:
            return "Application";

        case Input::KeyboardButton::Power:
            return "Power";

        case Input::KeyboardButton::KpEquals:
            return "KpEquals";

        case Input::KeyboardButton::F13:
            return "F13";

        case Input::KeyboardButton::F14:
            return "F14";

        case Input::KeyboardButton::F15:
            return "F15";

        case Input::KeyboardButton::F16:
            return "F16";

        case Input::KeyboardButton::F17:
            return "F17";

        case Input::KeyboardButton::F18:
            return "F18";

        case Input::KeyboardButton::F19:
            return "F19";

        case Input::KeyboardButton::F20:
            return "F20";

        case Input::KeyboardButton::F21:
            return "F21";

        case Input::KeyboardButton::F22:
            return "F22";

        case Input::KeyboardButton::F23:
            return "F23";

        case Input::KeyboardButton::F24:
            return "F24";

        case Input::KeyboardButton::Execute:
            return "Execute";

        case Input::KeyboardButton::Help:
            return "Help";

        case Input::KeyboardButton::Menu:
            return "Menu";

        case Input::KeyboardButton::Select:
            return "Select";

        case Input::KeyboardButton::Stop:
            return "Stop";

        case Input::KeyboardButton::Again:
            return "Again";

        case Input::KeyboardButton::Undo:
            return "Undo";

        case Input::KeyboardButton::Cut:
            return "Cut";

        case Input::KeyboardButton::Copy:
            return "Copy";

        case Input::KeyboardButton::Paste:
            return "Paste";

        case Input::KeyboardButton::Find:
            return "Find";

        case Input::KeyboardButton::Mute:
            return "Mute";

        case Input::KeyboardButton::Volumeup:
            return "Volumeup";

        case Input::KeyboardButton::Volumedown:
            return "Volumedown";

        case Input::KeyboardButton::KpComma:
            return "KpComma";

        case Input::KeyboardButton::KpEqualsas400:
            return "KpEqualsas400";

        case Input::KeyboardButton::International1:
            return "International1";

        case Input::KeyboardButton::International2:
            return "International2";

        case Input::KeyboardButton::International3:
            return "International3";

        case Input::KeyboardButton::International4:
            return "International4";

        case Input::KeyboardButton::International5:
            return "International5";

        case Input::KeyboardButton::International6:
            return "International6";

        case Input::KeyboardButton::International7:
            return "International7";

        case Input::KeyboardButton::International8:
            return "International8";

        case Input::KeyboardButton::International9:
            return "International9";

        case Input::KeyboardButton::Lang1:
            return "Lang1";

        case Input::KeyboardButton::Lang2:
            return "Lang2";

        case Input::KeyboardButton::Lang3:
            return "Lang3";

        case Input::KeyboardButton::Lang4:
            return "Lang4";

        case Input::KeyboardButton::Lang5:
            return "Lang5";

        case Input::KeyboardButton::Lang6:
            return "Lang6";

        case Input::KeyboardButton::Lang7:
            return "Lang7";

        case Input::KeyboardButton::Lang8:
            return "Lang8";

        case Input::KeyboardButton::Lang9:
            return "Lang9";

        case Input::KeyboardButton::Alterase:
            return "Alterase";

        case Input::KeyboardButton::Sysreq:
            return "Sysreq";

        case Input::KeyboardButton::Cancel:
            return "Cancel";

        case Input::KeyboardButton::Clear:
            return "Clear";

        case Input::KeyboardButton::Prior:
            return "Prior";

        case Input::KeyboardButton::Return2:
            return "Return2";

        case Input::KeyboardButton::Separator:
            return "Separator";

        case Input::KeyboardButton::Out:
            return "Out";

        case Input::KeyboardButton::Oper:
            return "Oper";

        case Input::KeyboardButton::Clearagain:
            return "Clearagain";

        case Input::KeyboardButton::Crsel:
            return "Crsel";

        case Input::KeyboardButton::Exsel:
            return "Exsel";

        case Input::KeyboardButton::Kp00:
            return "Kp00";

        case Input::KeyboardButton::Kp000:
            return "Kp000";

        case Input::KeyboardButton::Thousandsseparator:
            return "Thousandsseparator";

        case Input::KeyboardButton::Decimalseparator:
            return "Decimalseparator";

        case Input::KeyboardButton::Currencyunit:
            return "Currencyunit";

        case Input::KeyboardButton::Currencysubunit:
            return "Currencysubunit";

        case Input::KeyboardButton::KpLeftparen:
            return "KpLeftparen";

        case Input::KeyboardButton::KpRightparen:
            return "KpRightparen";

        case Input::KeyboardButton::KpLeftbrace:
            return "KpLeftbrace";

        case Input::KeyboardButton::KpRightbrace:
            return "KpRightbrace";

        case Input::KeyboardButton::KpTab:
            return "KpTab";

        case Input::KeyboardButton::KpBackspace:
            return "KpBackspace";

        case Input::KeyboardButton::KpA:
            return "KpA";

        case Input::KeyboardButton::KpB:
            return "KpB";

        case Input::KeyboardButton::KpC:
            return "KpC";

        case Input::KeyboardButton::KpD:
            return "KpD";

        case Input::KeyboardButton::KpE:
            return "KpE";

        case Input::KeyboardButton::KpF:
            return "KpF";

        case Input::KeyboardButton::KpXor:
            return "KpXor";

        case Input::KeyboardButton::KpPower:
            return "KpPower";

        case Input::KeyboardButton::KpPercent:
            return "KpPercent";

        case Input::KeyboardButton::KpLess:
            return "KpLess";

        case Input::KeyboardButton::KpGreater:
            return "KpGreater";

        case Input::KeyboardButton::KpAmpersand:
            return "KpAmpersand";

        case Input::KeyboardButton::KpDblampersand:
            return "KpDblampersand";

        case Input::KeyboardButton::KpVerticalbar:
            return "KpVerticalbar";

        case Input::KeyboardButton::KpDblverticalbar:
            return "KpDblverticalbar";

        case Input::KeyboardButton::KpColon:
            return "KpColon";

        case Input::KeyboardButton::KpHash:
            return "KpHash";

        case Input::KeyboardButton::KpSpace:
            return "KpSpace";

        case Input::KeyboardButton::KpAt:
            return "KpAt";

        case Input::KeyboardButton::KpExclam:
            return "KpExclam";

        case Input::KeyboardButton::KpMemstore:
            return "KpMemstore";

        case Input::KeyboardButton::KpMemrecall:
            return "KpMemrecall";

        case Input::KeyboardButton::KpMemclear:
            return "KpMemclear";

        case Input::KeyboardButton::KpMemadd:
            return "KpMemadd";

        case Input::KeyboardButton::KpMemsubtract:
            return "KpMemsubtract";

        case Input::KeyboardButton::KpMemmultiply:
            return "KpMemmultiply";

        case Input::KeyboardButton::KpMemdivide:
            return "KpMemdivide";

        case Input::KeyboardButton::KpPlusminus:
            return "KpPlusminus";

        case Input::KeyboardButton::KpClear:
            return "KpClear";

        case Input::KeyboardButton::KpClearentry:
            return "KpClearentry";

        case Input::KeyboardButton::KpBinary:
            return "KpBinary";

        case Input::KeyboardButton::KpOctal:
            return "KpOctal";

        case Input::KeyboardButton::KpDecimal:
            return "KpDecimal";

        case Input::KeyboardButton::KpHexadecimal:
            return "KpHexadecimal";

        case Input::KeyboardButton::LCtrl:
            return "LCtrl";

        case Input::KeyboardButton::LShift:
            return "LShift";

        case Input::KeyboardButton::LAlt:
            return "LAlt";

        case Input::KeyboardButton::LGui:
            return "LGui";

        case Input::KeyboardButton::RCtrl:
            return "RCtrl";

        case Input::KeyboardButton::RShift:
            return "RShift";

        case Input::KeyboardButton::RAlt:
            return "RAlt";

        case Input::KeyboardButton::RGui:
            return "RGui";

        case Input::KeyboardButton::Mode:
            return "Mode";

        case Input::KeyboardButton::Sleep:
            return "Sleep";

        case Input::KeyboardButton::Wake:
            return "Wake";

        case Input::KeyboardButton::ChannelIncrement:
            return "ChannelIncrement";

        case Input::KeyboardButton::ChannelDecrement:
            return "ChannelDecrement";

        case Input::KeyboardButton::MediaPlay:
            return "MediaPlay";

        case Input::KeyboardButton::MediaPause:
            return "MediaPause";

        case Input::KeyboardButton::MediaRecord:
            return "MediaRecord";

        case Input::KeyboardButton::MediaFastForward:
            return "MediaFastForward";

        case Input::KeyboardButton::MediaRewind:
            return "MediaRewind";

        case Input::KeyboardButton::MediaNextTrack:
            return "MediaNextTrack";

        case Input::KeyboardButton::MediaPreviousTrack:
            return "MediaPreviousTrack";

        case Input::KeyboardButton::MediaStop:
            return "MediaStop";

        case Input::KeyboardButton::MediaEject:
            return "MediaEject";

        case Input::KeyboardButton::MediaPlayPause:
            return "MediaPlayPause";

        case Input::KeyboardButton::MediaSelect:
            return "MediaSelect";

        case Input::KeyboardButton::AcNew:
            return "AcNew";

        case Input::KeyboardButton::AcOpen:
            return "AcOpen";

        case Input::KeyboardButton::AcClose:
            return "AcClose";

        case Input::KeyboardButton::AcExit:
            return "AcExit";

        case Input::KeyboardButton::AcSave:
            return "AcSave";

        case Input::KeyboardButton::AcPrint:
            return "AcPrint";

        case Input::KeyboardButton::AcProperties:
            return "AcProperties";

        case Input::KeyboardButton::AcSearch:
            return "AcSearch";

        case Input::KeyboardButton::AcHome:
            return "AcHome";

        case Input::KeyboardButton::AcBack:
            return "AcBack";

        case Input::KeyboardButton::AcForward:
            return "AcForward";

        case Input::KeyboardButton::AcStop:
            return "AcStop";

        case Input::KeyboardButton::AcRefresh:
            return "AcRefresh";

        case Input::KeyboardButton::AcBookmarks:
            return "AcBookmarks";

        case Input::KeyboardButton::Softleft:
            return "Softleft";

        case Input::KeyboardButton::Softright:
            return "Softright";

        case Input::KeyboardButton::Call:
            return "Call";

        case Input::KeyboardButton::Endcall:
            return "Endcall";

        default:
            return "";
        }
    }
}
