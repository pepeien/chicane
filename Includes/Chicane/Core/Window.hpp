#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Core/Window/CreateInfo.hpp"
#include "Chicane/Core/Window/Event.hpp"
#include "Chicane/Core/Window/Type.hpp"

namespace Chicane
{
    class CHICANE_CORE Window
    {
    public:
        using EventObservable   = Observable<WindowEvent>;
        using EventSubscription = Subscription<WindowEvent>;

        using SizeObservable    = Observable<Vec<2, int>>;
        using SizeSubscription  = Subscription<Vec<2, int>>;

    public:
        Window();
        ~Window();

    public:
        // Lifecycle
        void init(const WindowCreateInfo& inCreateInfo);
        bool run();

        // Settings
        void setTitle(const String& inTitle);
        void setIcon(const FileSystem::Path& inPath);

        const Vec<2, int>& getSize() const;
        void setSize(const Vec<2, int>& inValue);
        void setSize(int inWidth, int inHeight);

        const Vec<2, int>& getPosition() const;
        void setPosition(const Vec<2, int>& inValue);
        void setPosition(int inX, int inY);

        void setDisplay(std::uint32_t inIndex);

        void setType(WindowType inType);
        WindowType getType() const;

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
        void disableResizing(); // Only takes effect when the type is
                                // `Type::Windowed`

        bool isMinimized();

        // Event
        EventSubscription watchEvent(
            EventSubscription::NextCallback     inNext,
            EventSubscription::ErrorCallback    inError    = nullptr,
            EventSubscription::CompleteCallback inComplete = nullptr
        );
        SizeSubscription watchSize(
            SizeSubscription::NextCallback     inNext,
            SizeSubscription::ErrorCallback    inError    = nullptr,
            SizeSubscription::CompleteCallback inComplete = nullptr
        );

    private:
        // Settings
        void refreshSize();
        void refreshPosition();

        // Helpers
        void emmitWarning(const String& inMessage);
        void emmitError(const String& inMessage);

    private:
        void*            m_instance;

        String           m_title;
        FileSystem::Path m_icon;
        Vec<2, int>      m_size;
        int              m_display;
        WindowType       m_type;
        Vec<2, int>      m_position;

        bool             m_bIsFocused;
        bool             m_bIsResizable;
        bool             m_bIsMinimized; // Only takes effect when the type is
                                         // `WindowType::Windowed`

        EventObservable  m_eventObservable;
        SizeObservable   m_sizeObservable;
    };
}