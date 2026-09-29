#pragma once

#include "Chicane/Box/Mesh.hpp"

#include "Chicane/Core/FileSystem.hpp"
#include "Chicane/Core/Math/Bounds/3D.hpp"
#include "Chicane/Core/Math/Vertex.hpp"

#include "Chicane/Runtime/Scene/Trace/Shape.hpp"

namespace Chicane
{
    class CHICANE_RUNTIME SceneTraceShapeMesh : public SceneTraceShape
    {
    public:
        explicit SceneTraceShapeMesh(const Box::Mesh* inMesh);
        explicit SceneTraceShapeMesh(const FileSystem::Path& inMesh);
        SceneTraceShapeMesh() = default;

    public:
        bool isValid() const override;
        bool intersects(
            const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
        ) const override;

    public:
        const Vertex::List& getVertices() const;
        const Vertex::Indices& getIndices() const;
        const Bounds3D& getBounds() const;

    private:
        void ensureGeometry() const;

    public:
        const Box::Mesh* mesh = nullptr;

    private:
        mutable Vertex::List    m_vertices     = {};
        mutable Vertex::Indices m_indices      = {};
        mutable Bounds3D        m_bounds       = {};
        mutable bool            m_bHasGeometry = false;
    };
}
