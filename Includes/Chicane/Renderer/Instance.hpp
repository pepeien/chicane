#pragma once

#include "Chicane/Core/Event/Observable.hpp"
#include "Chicane/Core/Event/Subscription.hpp"
#include "Chicane/Core/View.hpp"
#include "Chicane/Core/Window.hpp"
#include "Chicane/Core/Window/Event.hpp"

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Draw/Draw2D.hpp"
#include "Chicane/Renderer/Draw/Draw3D.hpp"
#include "Chicane/Renderer/Draw/Data/Data2D.hpp"
#include "Chicane/Renderer/Draw/Data/Data3D.hpp"
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

            // Render
            void useCamera(const View& inData);

            void addLight(const View& inData);

            void draw(const DrawData2D& inData);

            void draw(const DrawData3D& inData);

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
            void reset();

            void resetViews();
            void resetCamera();
            void resetLights();

            bool areDrawsEmpty() const;
            void resetDraws();

            bool areDraws2DEmpty() const;
            void resetDraws2D();

            bool areDraws3DEmpty() const;
            void resetDraws3D();

        private:
            // Window
            Window*            m_window;

            // Draws
            Draw2D::List       m_draws2D;
            Vertex::List       m_vertices2D;
            Vertex::Indices    m_indices2D;

            Draw3D::List       m_draws3D;
            Vertex::List       m_vertices3D;
            Vertex::Indices    m_indices3D;

            View               m_camera;
            View::List         m_lights;

            // Settings
            Vec2               m_size;
            SizeObservable     m_sizeOberservable;

            Vec2               m_position;
            PositionObservable m_positionOberservable;
        };
    }
}