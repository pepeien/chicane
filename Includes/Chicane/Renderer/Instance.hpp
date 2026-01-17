#pragma once

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Viewport.hpp"

namespace Chicane
{
    namespace Renderer
    {
        class CHICANE_RENDERER Instance
        {
        public:
            using ViewportObservable   = EventObservable<Viewport>;
            using ViewportSubscription = EventSubscription<Viewport>;

            using SizeObservable       = EventObservable<const Vec2&>;
            using SizeSubscription     = EventSubscription<const Vec2&>;

            using PositionObservable   = EventObservable<const Vec2&>;
            using PositionSubscription = EventSubscription<const Vec2&>;

        public:
            Instance();
            ~Instance();

        public:
            // Status
            bool canRender() const;

            // Lifecycle
            void init(Window* inWindow);
            void render();
            void destroy();

            // Settings
            const Vec2& getSize() const;
            void setSize(const Vec2& inValue);
            void setSize(float inWidth, float inHeight);
            SizeSubscription watchSize(
                SizeObservable::NextCallback     inNext,
                SizeObservable::ErrorCallback    inError    = nullptr,
                SizeObservable::CompleteCallback inComplete = nullptr
            );

            const Vec2& getPosition() const;
            void setPosition(const Vec2& inValue);
            void setPosition(float inX, float inY);
            PositionSubscription watchPosition(
                PositionObservable::NextCallback     inNext,
                PositionObservable::ErrorCallback    inError    = nullptr,
                PositionObservable::CompleteCallback inComplete = nullptr
            );

            // Window
            Window* getWindow() const;
            void setWindow(Window* inWindow);
            void handle(WindowEvent inEvent);

        private:
            // Settings
            Vec2               m_size;
            SizeObservable     m_sizeOberservable;

            Vec2               m_position;
            PositionObservable m_positionOberservable;

            // Window
            Window*            m_window;
        };
    }
}