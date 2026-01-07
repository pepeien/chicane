#pragma once

#include "Chicane/Core/Essential.hpp"

namespace Chicane
{
    // Extracted from `SDL3/SDL_EventType`
    enum class WindowEventType : std::uint16_t
    {
        First = 0,

        // Application events
        Quit = 0x100,
        Terminating,
        LowMemory,
        WillEnterBackground,
        DidEnterBackground,
        WillEnterForeground,
        DidEnterForeground,
        LocaleChanged,
        SystemThemeChanged,

        // Display events
        DisplayOrientation = 0x151,
        DisplayAdded,
        DisplayRemoved,
        DisplayMoved,
        DisplayDesktopModeChanged,
        DisplayCurrentModeChanged,
        DisplayContentScaleChanged,

        // Window events
        WindowShown = 0x202,
        WindowHidden,
        WindowExposed,
        WindowMoved,
        WindowResized,
        WindowPixelSizeChanged,
        WindowMetalViewResized,
        WindowMinimized,
        WindowMaximized,
        WindowRestored,
        WindowMouseEnter,
        WindowMouseLeave,
        WindowFocusGained,
        WindowFocusLost,
        WindowCloseRequested,
        WindowHitTest,
        WindowIccProfChanged,
        WindowDisplayChanged,
        WindowDisplayScaleChanged,
        WindowSafeAreaChanged,
        WindowOccluded,
        WindowEnterFullscreen,
        WindowLeaveFullscreen,
        WindowDestroyed,
        WindowHdrStateChanged,

        // Keyboard events
        KeyDown = 0x300,
        KeyUp,
        TextEditing,
        TextInput,
        KeymapChanged,
        KeyboardAdded,
        KeyboardRemoved,
        TextEditingCandidates,

        // Mouse events
        MouseMotion = 0x400,
        MouseButtonDown,
        MouseButtonUp,
        MouseWheel,
        MouseAdded,
        MouseRemoved,

        // Joystick events
        JoystickAxisMotion = 0x600,
        JoystickBallMotion,
        JoystickHatMotion,
        JoystickButtonDown,
        JoystickButtonUp,
        JoystickAdded,
        JoystickRemoved,
        JoystickBatteryUpdated,
        JoystickUpdateComplete,

        // Gamepad events
        GamepadAxisMotion = 0x650,
        GamepadButtonDown,
        GamepadButtonUp,
        GamepadAdded,
        GamepadRemoved,
        GamepadRemapped,
        GamepadTouchpadDown,
        GamepadTouchpadMotion,
        GamepadTouchpadUp,
        GamepadSensorUpdate,
        GamepadUpdateComplete,
        GamepadSteamHandleUpdated,

        // Touch events
        FingerDown = 0x700,
        FingerUp,
        FingerMotion,
        FingerCanceled,

        // Clipboard events
        ClipboardUpdate = 0x900,

        // Drag and drop events
        DropFile = 0x1000,
        DropText,
        DropBegin,
        DropComplete,
        DropPosition,

        // Audio hotplug events
        AudioDeviceAdded = 0x1100,
        AudioDeviceRemoved,
        AudioDeviceFormatChanged,

        // Sensor events
        SensorUpdate = 0x1200,

        // Pen events
        PenProximityIn = 0x1300,
        PenProximityOut,
        PenDown,
        PenUp,
        PenButtonDown,
        PenButtonUp,
        PenMotion,
        PenAxis,

        // Camera events
        CameraDeviceAdded = 0x1400,
        CameraDeviceRemoved,
        CameraDeviceApproved,
        CameraDeviceDenied,

        // Render events
        RenderTargetsReset = 0x2000,
        RenderDeviceReset,
        RenderDeviceLost,

        // Private platform events
        Private0 = 0x4000,
        Private1,
        Private2,
        Private3,

        // Internal
        PollSentinel = 0x7F00,

        // User-defined
        User = 0x8000,

        // Bounds
        Last = 0xFFFF
    };
}