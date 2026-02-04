#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Core/Window/Settings.hpp"
#include "Chicane/Core/Window/Type.hpp"

namespace Chicane
{
    using WindowEventObservable   = EventObservable<WindowEvent>;
    using WindowEventSubscription = EventSubscription<WindowEvent>;

    using WindowSizeObservable   = EventObservable<Vec<2, std::uint32_t>>;
    using WindowSizeSubscription = EventSubscription<Vec<2, std::uint32_t>>;

    using WindowBackendObservable   = EventObservable<WindowBackend>;
    using WindowBackendSubscription = EventSubscription<WindowBackend>;

    class Application;

    class CHICANE_CORE Window
    {
        friend Application;

    public:
        Window();
        ~Window();

    public:
        // Lifecycle
        void init(const WindowSettings& inSettings);
        bool run();
        void restart();

        // Settings
        void setTitle(const String& inTitle);
        void setIcon(const FileSystem::Path& inPath);

        const Vec<2, std::uint32_t>& getSize() const;
        void setSize(const Vec<2, std::uint32_t>& inValue);
        void setSize(int inWidth, int inHeight);

        const Vec<2, std::uint32_t>& getPosition() const;
        void setPosition(const Vec<2, std::uint32_t>& inValue);
        void setPosition(int inX, int inY);

        std::uint32_t getDisplay() const;
        void setDisplay(std::uint32_t inIndex);

        WindowType getType() const;
        void setType(WindowType inType);

        WindowBackend getBackend() const;

        bool hasInstance() const;
        void initInstance();
        void* getInstance() const; // SDL_Window*
        void destroyInstance();

        // Status
        bool isFocused() const;
        void switchFocus();
        void focus();
        void blur();

        bool isResizable() const;
        void enableResizing();  // Only takes effect when the type is `Type::Windowed`
        void disableResizing(); // Only takes effect when the type is `Type::Windowed`

        bool isMinimized() const;

        // Event
        WindowEventSubscription watchEvent(
            WindowEventSubscription::NextCallback     inNext,
            WindowEventSubscription::ErrorCallback    inError    = nullptr,
            WindowEventSubscription::CompleteCallback inComplete = nullptr
        );
        WindowSizeSubscription watchSize(
            WindowSizeSubscription::NextCallback     inNext,
            WindowSizeSubscription::ErrorCallback    inError    = nullptr,
            WindowSizeSubscription::CompleteCallback inComplete = nullptr
        );
        WindowBackendSubscription watchBackend(
            WindowBackendSubscription::NextCallback     inNext,
            WindowBackendSubscription::ErrorCallback    inError    = nullptr,
            WindowBackendSubscription::CompleteCallback inComplete = nullptr
        );

    protected:
        void setBackend(WindowBackend inBackend);

    private:
        // Settings
        void refreshSize();
        void refreshPosition();

        // Helpers
        void emmitWarning(const String& inMessage);
        void emmitError(const String& inMessage);

    private:
        void*                   m_instance;

        WindowSettings          m_settings;

        bool                    m_bIsFocused;
        bool                    m_bIsResizable;
        bool                    m_bIsMinimized; // Only takes effect when the type is `WindowType::Windowed`

        WindowEventObservable   m_eventObservable;
        WindowSizeObservable    m_sizeObservable;
        WindowBackendObservable m_backendObservable;
    };
}