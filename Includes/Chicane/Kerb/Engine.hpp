#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Kerb.hpp"

namespace Chicane
{
    namespace Kerb
    {
        class CHICANE_KERB Engine
        {
        public:
            static Engine& getInstance()
            {
                static Engine instance;

                return instance;
            }

        public:
            Engine(const Engine&) = delete;
            Engine& operator=(const Engine&) = delete;

            Engine(Engine&&) = delete;
            Engine& operator=(Engine&&) = delete;

        public:
            JPH::ShapeSettings::ShapeResult createShapeFromMesh(const std::vector<Vertex>& inVertices, const std::vector<std::uint32_t>& inIndices) const;

        private:
            Engine();

            ~Engine();
        };
    }
}