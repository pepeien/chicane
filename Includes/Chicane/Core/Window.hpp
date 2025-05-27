#pragma once

#include "Chicane/Core/Gamepad.hpp"
#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Core/Filesystem.hpp"
#include "Chicane/Core/Keyboard.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Mouse.hpp"

namespace Chicane
{
    class CHICANE_CORE Window
    {
    public:
        enum class Type : std::uint8_t
        {
            Windowed,
            WindowedBorderless,
            Fullscreen,
        };

        enum class EventType : std::uint16_t
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

        enum class Renderer : std::uint8_t
        {
            Undefined,
            OpenGL,
            Vulkan
        };

    public:
        struct CreateInfo
        {
        public:
            std::string      title    = "";
            FileSystem::Path icon     = "";
            Vec<2, int>      size     = Vec<2, int>(0);
            int              display  = 0;
            Type             type     = Type::Windowed;
            Renderer         renderer = Renderer::Undefined;
        };

        struct Event
        {
        public:
            EventType type = EventType::First;
            void*     data = nullptr; // SDL_Event
        };

    public:
        using EventObservable   = Observable<Event>;
        using EventSubscription = Subscription<Event>;

        using SizeObservable   = Observable<Vec<2, int>>;
        using SizeSubscription = Subscription<Vec<2, int>>;

    public:
        Window();
        ~Window();

    public:
        // Lifecycle
        void init(const CreateInfo& inCreateInfo);
        bool update();

        // Settings
        void setTitle(const std::string& inTitle);
        void setIcon(const FileSystem::Path& inPath);

        const Vec<2, int>& getSize() const;
        void setSize(const Vec<2, int>& inValue);
        void setSize(int inWidth, int inHeight);

        const Vec<2, int>& getPosition() const;
        void setPosition(const Vec<2, int>& inValue);
        void setPosition(int inX, int inY);

        void setDisplay(int inIndex);

        void setType(Type inType);
        Type getType() const;

        // Status
        void* getInstance() const;

        bool wasCreated() const;
        void destroy();

        bool isFocused() const;
        void switchFocus();
        void focus();
        void blur();
    
        bool isResizable();
        void enableResizing();  // Only takes effect when the type is `Type::Windowed`
        void disableResizing(); // Only takes effect when the type is `Type::Windowed`

        bool isMinimized();

        // Event
        EventSubscription watchEvent(
            EventSubscription::NextCallback inNext,
            EventSubscription::ErrorCallback inError = nullptr,
            EventSubscription::CompleteCallback inComplete = nullptr
        );
        SizeSubscription watchSize(
            SizeSubscription::NextCallback inNext,
            SizeSubscription::ErrorCallback inError = nullptr,
            SizeSubscription::CompleteCallback inComplete = nullptr
        );

    private:
        // Settings
        void refreshSize();
        void refreshPosition();

        // Helpers
        void emmitWarning(const std::string& inMessage);
        void emmitError(const std::string& inMessage);

    private:
        void*            m_instance;

        std::string      m_title;
        FileSystem::Path m_icon;
        Vec<2, int>      m_size;
        int              m_display;
        Type             m_type;
        Vec<2, int>      m_position;

        bool             m_bIsFocused;
        bool             m_bIsResizable;
        bool             m_bIsMinimized; // Only takes effect when the type is `Window::Type::Windowed`

        EventObservable  m_eventObservable;
        SizeObservable   m_sizeObservable;
    };
}