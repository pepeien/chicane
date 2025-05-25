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
            Instance(const std::string& inId);

            virtual ~Instance() = default;

        protected:
            virtual bool onInit() { return false; }
            virtual bool onDestroy() { return true; }
            virtual bool onRebuild() { return true; }
            virtual bool onSetup() { return true; }
            virtual void onRender(void* outData) { return; }
            virtual void onEvent(const SDL_Event& inEvent) { return; }

        public:  
            void init();
            void destroy();
            void rebuild();
            void setup();
            void render(void* outData);
            void handle(const SDL_Event& inEvent);

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