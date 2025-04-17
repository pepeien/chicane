#pragma once

#include "Core/Essentials.hpp"
#include "Core/Event.hpp"
#include "Core/Math/Vec.hpp"
#include "Core/Window/CreateInfo.hpp"
#include "Core/Window/Type.hpp"
#include "Runtime/Renderer/Type.hpp"

namespace Chicane
{
    namespace Window
    {
        class CHICANE Instance
        {
        public:
            Instance(const CreateInfo& inCreateInfo, Renderer::Type inRendererType);
            ~Instance();

        public:
            // Event
            void onEvent(const SDL_Event& inEvent);

            // Settings
            void setTitle(const std::string& inTitle);
            void setIcon(const std::string& inIconPath);

            const Vec<2, int>& getSize() const;
            void setSize(const Vec<2, int>& inSize);
            void setSize(int inWidth, int inHeight);

            const Vec<2, int>& getPosition() const;
            void setPosition(const Vec<2, int>& inPosition);
            void setPosition(int inX, int inY);

            void setDisplay(int inMonitorIndex);

            void setType(Type inType);
            Type getType() const;

            // Status
            bool isFocused() const;
            void switchFocus();
            void focus();
            void blur();
    
            bool isResizable();
            void enableResizing();  // Only takes effect when the type is `Type::Windowed`
            void disableResizing(); // Only takes effect when the type is `Type::Windowed`

            bool isMinimized();

            // Event
            Subscription<const Vec<2, int>&>* watchSize(
                std::function<void (const Vec<2, int>&)> inNext,
                std::function<void (const std::string&)> inError = nullptr,
                std::function<void ()> inComplete = nullptr
            );

        private:
            // Settings
            void refreshSize();
            void refreshPosition();

        public:
            SDL_Window*                                     instance;

        private:
            // Settings
            Type                                            m_type;
            Vec<2, int>                                     m_size;
            Vec<2, int>                                     m_position;

            // Status
            bool                                            m_bIsFocused;
            bool                                            m_bIsResizable;
            bool                                            m_bIsMinimized; // Only takes effect when the type is `Window::Type::Windowed`

            // Event
            std::unique_ptr<Observable<const Vec<2, int>&>> m_sizeObservable;
        };
    }
}