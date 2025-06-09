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
            using Super = Instance;

        public:
            Instance();
            Instance(const String& inId);

            virtual ~Instance() = default;

        protected:
            virtual bool onInit() { return false; }
            virtual bool onDestroy() { return true; }
            virtual bool onRebuild() { return true; }
            virtual bool onSetup() { return true; }
            virtual void onRender(void* outData) { return; }
            virtual void onEvent(const Window::Event& inEvent) { return; }

        public:  
            void init();
            void destroy();
            void rebuild();
            void setup();
            void render(void* outData);
            void handle(const Window::Event& inEvent);

        public:
            bool is(Status inStatus) const;
            Status getStatus() const;

            const String& getId() const;

        protected:
            void setStatus(Status inStatus);

        protected:
            String m_id;
            Status      m_status;
        };
    }
}