#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Renderer/Layer/Status.hpp"

namespace Chicane
{
    namespace Layer
    {
        class CHICANE_RUNTIME Instance
        {
        protected:
            typedef Instance Super;

        public:
            Instance();
            Instance(const std::string& inId);

            virtual ~Instance() = default;

        public:
            // Builds the Layer
            virtual bool onBuild() { return false; }
            // Destroys any resource that can be rebuilt at rebuild
            virtual bool onDestroy() { return true; }
            // Rebuilds resources of the layer
            virtual bool onRebuild() { return true; }
            // Setup layer for rendering
            virtual bool onSetup() { return true; }
            // Renders layer
            virtual void onRender(void* outData) { return; }

            // Event
            virtual void onEvent(const SDL_Event& inEvent) { return; }

        public:  
            void build();
            void destroy();
            void rebuild();
            void setup();
            void render(void* outData);

        public:
            bool is(Status inStatus) const;
            Status getStatus() const;

            const std::string& getId() const;

        protected:
            void setStatus(Status inStatus);

        protected:
            std::string m_id;
            Status      m_status;
        };
    }
}