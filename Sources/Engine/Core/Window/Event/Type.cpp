#include "Chicane/Core/Window/Event/Type.hpp"

namespace Chicane
{
    String toString(WindowEventType inValue)
    {
        switch (inValue)
        {
        case WindowEventType::First:
            return "First";

        case WindowEventType::Quit:
            return "Quit";

        case WindowEventType::Terminating:
            return "Terminating";

        case WindowEventType::LowMemory:
            return "LowMemory";

        case WindowEventType::WillEnterBackground:
            return "WillEnterBackground";

        case WindowEventType::DidEnterBackground:
            return "DidEnterBackground";

        case WindowEventType::WillEnterForeground:
            return "WillEnterForeground";

        case WindowEventType::DidEnterForeground:
            return "DidEnterForeground";

        case WindowEventType::LocaleChanged:
            return "LocaleChanged";

        case WindowEventType::SystemThemeChanged:
            return "SystemThemeChanged";

        case WindowEventType::DisplayOrientation:
            return "DisplayOrientation";

        case WindowEventType::DisplayAdded:
            return "DisplayAdded";

        case WindowEventType::DisplayRemoved:
            return "DisplayRemoved";

        case WindowEventType::DisplayMoved:
            return "DisplayMoved";

        case WindowEventType::DisplayDesktopModeChanged:
            return "DisplayDesktopModeChanged";

        case WindowEventType::DisplayCurrentModeChanged:
            return "DisplayCurrentModeChanged";

        case WindowEventType::DisplayContentScaleChanged:
            return "DisplayContentScaleChanged";

        case WindowEventType::WindowShown:
            return "WindowShown";

        case WindowEventType::WindowHidden:
            return "WindowHidden";

        case WindowEventType::WindowExposed:
            return "WindowExposed";

        case WindowEventType::WindowMoved:
            return "WindowMoved";

        case WindowEventType::WindowResized:
            return "WindowResized";

        case WindowEventType::WindowPixelSizeChanged:
            return "WindowPixelSizeChanged";

        case WindowEventType::WindowMetalViewResized:
            return "WindowMetalViewResized";

        case WindowEventType::WindowMinimized:
            return "WindowMinimized";

        case WindowEventType::WindowMaximized:
            return "WindowMaximized";

        case WindowEventType::WindowRestored:
            return "WindowRestored";

        case WindowEventType::WindowMouseEnter:
            return "WindowMouseEnter";

        case WindowEventType::WindowMouseLeave:
            return "WindowMouseLeave";

        case WindowEventType::WindowFocusGained:
            return "WindowFocusGained";

        case WindowEventType::WindowFocusLost:
            return "WindowFocusLost";

        case WindowEventType::WindowCloseRequested:
            return "WindowCloseRequested";

        case WindowEventType::WindowHitTest:
            return "WindowHitTest";

        case WindowEventType::WindowIccProfChanged:
            return "WindowIccProfChanged";

        case WindowEventType::WindowDisplayChanged:
            return "WindowDisplayChanged";

        case WindowEventType::WindowDisplayScaleChanged:
            return "WindowDisplayScaleChanged";

        case WindowEventType::WindowSafeAreaChanged:
            return "WindowSafeAreaChanged";

        case WindowEventType::WindowOccluded:
            return "WindowOccluded";

        case WindowEventType::WindowEnterFullscreen:
            return "WindowEnterFullscreen";

        case WindowEventType::WindowLeaveFullscreen:
            return "WindowLeaveFullscreen";

        case WindowEventType::WindowDestroyed:
            return "WindowDestroyed";

        case WindowEventType::WindowHdrStateChanged:
            return "WindowHdrStateChanged";

        case WindowEventType::KeyDown:
            return "KeyDown";

        case WindowEventType::KeyUp:
            return "KeyUp";

        case WindowEventType::TextEditing:
            return "TextEditing";

        case WindowEventType::TextInput:
            return "TextInput";

        case WindowEventType::KeymapChanged:
            return "KeymapChanged";

        case WindowEventType::KeyboardAdded:
            return "KeyboardAdded";

        case WindowEventType::KeyboardRemoved:
            return "KeyboardRemoved";

        case WindowEventType::TextEditingCandidates:
            return "TextEditingCandidates";

        case WindowEventType::MouseMotion:
            return "MouseMotion";

        case WindowEventType::MouseButtonDown:
            return "MouseButtonDown";

        case WindowEventType::MouseButtonUp:
            return "MouseButtonUp";

        case WindowEventType::MouseWheel:
            return "MouseWheel";

        case WindowEventType::MouseAdded:
            return "MouseAdded";

        case WindowEventType::MouseRemoved:
            return "MouseRemoved";

        case WindowEventType::JoystickAxisMotion:
            return "JoystickAxisMotion";

        case WindowEventType::JoystickBallMotion:
            return "JoystickBallMotion";

        case WindowEventType::JoystickHatMotion:
            return "JoystickHatMotion";

        case WindowEventType::JoystickButtonDown:
            return "JoystickButtonDown";

        case WindowEventType::JoystickButtonUp:
            return "JoystickButtonUp";

        case WindowEventType::JoystickAdded:
            return "JoystickAdded";

        case WindowEventType::JoystickRemoved:
            return "JoystickRemoved";

        case WindowEventType::JoystickBatteryUpdated:
            return "JoystickBatteryUpdated";

        case WindowEventType::JoystickUpdateComplete:
            return "JoystickUpdateComplete";

        case WindowEventType::GamepadAxisMotion:
            return "GamepadAxisMotion";

        case WindowEventType::GamepadButtonDown:
            return "GamepadButtonDown";

        case WindowEventType::GamepadButtonUp:
            return "GamepadButtonUp";

        case WindowEventType::GamepadAdded:
            return "GamepadAdded";

        case WindowEventType::GamepadRemoved:
            return "GamepadRemoved";

        case WindowEventType::GamepadRemapped:
            return "GamepadRemapped";

        case WindowEventType::GamepadTouchpadDown:
            return "GamepadTouchpadDown";

        case WindowEventType::GamepadTouchpadMotion:
            return "GamepadTouchpadMotion";

        case WindowEventType::GamepadTouchpadUp:
            return "GamepadTouchpadUp";

        case WindowEventType::GamepadSensorUpdate:
            return "GamepadSensorUpdate";

        case WindowEventType::GamepadUpdateComplete:
            return "GamepadUpdateComplete";

        case WindowEventType::GamepadSteamHandleUpdated:
            return "GamepadSteamHandleUpdated";

        case WindowEventType::FingerDown:
            return "FingerDown";

        case WindowEventType::FingerUp:
            return "FingerUp";

        case WindowEventType::FingerMotion:
            return "FingerMotion";

        case WindowEventType::FingerCanceled:
            return "FingerCanceled";

        case WindowEventType::ClipboardUpdate:
            return "ClipboardUpdate";

        case WindowEventType::DropFile:
            return "DropFile";

        case WindowEventType::DropText:
            return "DropText";

        case WindowEventType::DropBegin:
            return "DropBegin";

        case WindowEventType::DropComplete:
            return "DropComplete";

        case WindowEventType::DropPosition:
            return "DropPosition";

        case WindowEventType::AudioDeviceAdded:
            return "AudioDeviceAdded";

        case WindowEventType::AudioDeviceRemoved:
            return "AudioDeviceRemoved";

        case WindowEventType::AudioDeviceFormatChanged:
            return "AudioDeviceFormatChanged";

        case WindowEventType::SensorUpdate:
            return "SensorUpdate";

        case WindowEventType::PenProximityIn:
            return "PenProximityIn";

        case WindowEventType::PenProximityOut:
            return "PenProximityOut";

        case WindowEventType::PenDown:
            return "PenDown";

        case WindowEventType::PenUp:
            return "PenUp";

        case WindowEventType::PenButtonDown:
            return "PenButtonDown";

        case WindowEventType::PenButtonUp:
            return "PenButtonUp";

        case WindowEventType::PenMotion:
            return "PenMotion";

        case WindowEventType::PenAxis:
            return "PenAxis";

        case WindowEventType::CameraDeviceAdded:
            return "CameraDeviceAdded";

        case WindowEventType::CameraDeviceRemoved:
            return "CameraDeviceRemoved";

        case WindowEventType::CameraDeviceApproved:
            return "CameraDeviceApproved";

        case WindowEventType::CameraDeviceDenied:
            return "CameraDeviceDenied";

        case WindowEventType::RenderTargetsReset:
            return "RenderTargetsReset";

        case WindowEventType::RenderDeviceReset:
            return "RenderDeviceReset";

        case WindowEventType::RenderDeviceLost:
            return "RenderDeviceLost";

        case WindowEventType::Private0:
            return "Private0";

        case WindowEventType::Private1:
            return "Private1";

        case WindowEventType::Private2:
            return "Private2";

        case WindowEventType::Private3:
            return "Private3";

        case WindowEventType::PollSentinel:
            return "PollSentinel";

        case WindowEventType::User:
            return "User";

        case WindowEventType::Last:
            return "Last";

        default:
            return "";
        }
    }
}
