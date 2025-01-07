#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    class Layer
    {
    public:
        enum class Status : std::uint8_t
        {
            Idle,
            Initialized,
            Running
        };

        enum class PushStrategy : std::uint8_t
        {
            Front,
            Back,
            BeforeLayer, // Require to have the ref layer `ID`
            AfterLayer   // Require to have the ref layer `ID`
        };

    public:
        Layer()
            : Layer("Unamed")
        {}
        Layer(const std::string& inId)
            : m_id(inId),
              m_status(Status::Idle)
        {}
        virtual ~Layer() = default;

    public:
        // Initialization
        virtual void build() { return; }
        virtual void destroy() { return; }
        virtual void rebuild() { return; }

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