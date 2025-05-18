#pragma once

#include "Chicane/Runtime/Essential.hpp"
#include "Chicane/Runtime/Renderer/Layer/Status.hpp"

namespace Chicane
{
    namespace Layer
    {
        class CHICANE_RUNTIME Instance
        {
        public:
            Instance()
                : Instance("Unamed")
            {}

            Instance(const std::string& inId)
                : m_id(inId),
                  m_status(Status::Offline)
            {}

            virtual ~Instance() = default;

        public:
            // Initialization
            virtual void build() { setStatus(Layer::Status::Running); }
            virtual void destroy() { setStatus(Layer::Status::Idle); }
            virtual void rebuild() { setStatus(Layer::Status::Running); }

            // Event
            virtual void onEvent(const SDL_Event& inEvent) { return; }

            // Rendering
            virtual void setup(void* outData) { return; }
            virtual void render(void* outData) { return; }

        public:
            bool is(Status inStatus) const { return m_status == inStatus; };

            const std::string& getId() const { return m_id; }

        protected:
            void setStatus(Status inStatus) { m_status = inStatus; };

        protected:
            std::string m_id;
            Status      m_status;
        };
    }
}