#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Math.hpp"
#include "Chicane/Renderer/Layer.hpp"

namespace Chicane
{
    class Window
    {
    public:
        enum class Type : uint8_t
        {
            Windowed,
            WindowedBorderless,
            Fullscreen,
        };

        struct CreateInfo
        {
        public:
            std::string title        = "";
            std::string icon         = ""; // Optional `file path`
            Vec<2, int> resolution   = Vec<2, int>(0);
            int         display      = 0;
            Type        type         = Type::Windowed;
            bool        bIsFocused   = false;
            bool        bIsResizable = true; // Only takes effect when the type is `Type::Windowed`
        };

    public:
        Window(const CreateInfo& inCreateInfo);
        ~Window();

    public:
        // Event
        void onEvent(const SDL_Event& inEvent);

        // Settings
        bool isFocused() const;
        void switchFocus();
        void focus();
        void blur();

        void setTitle(const std::string& inTitle);
        void setIcon(const std::string& inIconPath);

        const Vec<2, int>& getSize() const;
        void setSize(const Vec<2, int>& inSize);
        void setSize(int inWidth, int inHeight);

        const Vec<2, int>& getDrawableSize() const;
        void setDrawableSize(const Vec<2, int>& inSize);
        void setDrawableSize(int inWidth, int inHeight);

        const Vec<2, int>& getPosition() const;
        void setPosition(const Vec<2, int>& inPosition);
        void setPosition(int inX, int inY);

        void setViewport(
            const Vec<2, std::uint32_t>& inSize,
            const Vec<2, float>& inPosition = Vec2Zero
        );

        void setDisplay(int inMonitorIndex);

        void setType(Type inType);
        Type getType() const;

        bool isResizable();
        void enableResizing();  // Only takes effect when the type is `Type::Windowed`
        void disableResizing(); // Only takes effect when the type is `Type::Windowed`

        bool isMinimized();
    
    public:
        SDL_Window* instance;

    private:
        // Settings
        void refreshSize();
        void refreshDrawableSize();
        void refreshPosition();

    private:
        // Settings
        Type         m_type;
        Vec<2, int>  m_size;
        Vec<2, int>  m_drawableSize;
        Vec<2, int>  m_position;

        bool         m_bIsFocused;
        bool         m_bIsResizable;
        bool         m_bIsMinimized; // Only takes effect when the type is `Window::Type::Windowed`
    };
}