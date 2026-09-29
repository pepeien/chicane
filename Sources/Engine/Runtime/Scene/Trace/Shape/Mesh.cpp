#include "Chicane/Runtime/Scene/Trace/Shape/Mesh.hpp"

#include "Chicane/Box/Model.hpp"
#include "Chicane/Core/Math/Mat/Mat4.hpp"
#include "Chicane/Runtime/Scene/Trace/Shape/Utility.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Chicane
{
    SceneTraceShapeMesh::SceneTraceShapeMesh(const Box::Mesh* inMesh)
        : mesh(inMesh)
    {}

    SceneTraceShapeMesh::SceneTraceShapeMesh(const FileSystem::Path& inMesh)
        : SceneTraceShapeMesh(inMesh.isEmpty() ? nullptr : Box::load<Box::Mesh>(inMesh))
    {}

    bool SceneTraceShapeMesh::isValid() const
    {
        ensureGeometry();

        return mesh && !m_vertices.empty();
    }

    bool SceneTraceShapeMesh::intersects(
        const Vec3& inOrigin, const Vec3& inDestination, const Bounds3D& inBounds, float& outEnter
    ) const
    {
        if (!isValid())
        {
            return false;
        }

        const float scale = SceneTraceShapeUtility::axisLength(inOrigin, inDestination);
        if (scale <= FLT_EPSILON)
        {
            return false;
        }

        const Mat4 model = Mat4::sTranslate(inOrigin) * Mat4::sScale(Vec3(scale));

        Bounds3D bounds = getBounds();
        bounds.transform(model);
        if (!bounds.intersects(inBounds))
        {
            return false;
        }

        SceneTraceShapeUtility::closestPointOnSegment(inBounds.getCenter(), inOrigin, inDestination, outEnter);

        return true;
    }

    const Vertex::List& SceneTraceShapeMesh::getVertices() const
    {
        ensureGeometry();

        return m_vertices;
    }

    const Vertex::Indices& SceneTraceShapeMesh::getIndices() const
    {
        ensureGeometry();

        return m_indices;
    }

    const Bounds3D& SceneTraceShapeMesh::getBounds() const
    {
        ensureGeometry();

        return m_bounds;
    }

    void SceneTraceShapeMesh::ensureGeometry() const
    {
        if (m_bHasGeometry)
        {
            return;
        }

        m_bHasGeometry = true;
        m_vertices.clear();
        m_indices.clear();
        m_bounds = Bounds3D();

        if (!mesh)
        {
            return;
        }

        for (const Box::MeshGroup& group : mesh->getGroups())
        {
            const Box::Model* model = Box::load<Box::Model>(group.getModel().getSource());
            if (!model)
            {
                model = Box::Model::sGetDefault();
            }

            if (!model)
            {
                continue;
            }

            const Box::ModelParsed& parsed = model->getModel(group.getModel().getReference());
            const Mat4&             local  = group.getModelMatrix();
            const Vertex::Index     offset = static_cast<Vertex::Index>(m_vertices.size());

            Vertex::List transformed;
            transformed.reserve(parsed.vertices.size());
            for (const Vertex& vertex : parsed.vertices)
            {
                Vertex copy   = vertex;
                copy.position = local * vertex.position;
                transformed.push_back(copy);
            }

            m_vertices.insert(m_vertices.end(), transformed.begin(), transformed.end());
            m_bounds.add(transformed);

            if (parsed.indices.empty())
            {
                continue;
            }

            m_indices.reserve(m_indices.size() + parsed.indices.size());
            for (const Vertex::Index index : parsed.indices)
            {
                m_indices.push_back(offset + index);
            }
        }
    }
}
