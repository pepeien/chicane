#pragma once

#include <memory>
#include <vector>

#include "Chicane/Core/Math/Vec/Vec2.hpp"
#include "Chicane/Core/Math/Vertex.hpp"
#include "Chicane/Core/String.hpp"

#include "Chicane/Grid.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID Primitive
        {
        public:
            friend inline bool operator==(const Primitive& inLeft, const Primitive& inRight)
            {
                return (inLeft.vertices == inRight.vertices && inLeft.indices == inRight.indices);
            }

        public:
            bool isEmpty() const;
            bool hasGlyph() const;

            void clear();
            void seal();

            const std::shared_ptr<const Vertex::List>& getSharedVertices() const;
            const std::shared_ptr<const Vertex::Indices>& getSharedIndices() const;
            const std::shared_ptr<const std::vector<Vec2>>& getSharedOutline() const;

        public:
            String            reference  = String::sEmpty();
            Vertex::List      vertices   = {};
            Vertex::Indices   indices    = {};
            String            glyph      = String::sEmpty();
            float             dilation   = 0.0f;
            Vec2              outlineMin = Vec2::sZero();
            Vec2              outlineMax = Vec2::sZero();
            std::vector<Vec2> outline    = {};

        private:
            std::shared_ptr<const Vertex::List>      m_sharedVertices = nullptr;
            std::shared_ptr<const Vertex::Indices>   m_sharedIndices  = nullptr;
            std::shared_ptr<const std::vector<Vec2>> m_sharedOutline  = nullptr;
        };
    }
}