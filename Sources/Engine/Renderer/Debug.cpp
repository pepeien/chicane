#include "Chicane/Renderer/Debug.hpp"

#include <algorithm>
#include <cmath>

namespace Chicane
{
    namespace Renderer
    {
        namespace Debug
        {
            namespace
            {
                constexpr int   kRingSegments = 16;
                constexpr int   kSweepRings   = 4;
                constexpr float kPi           = 3.14159265358979323846f;

                float lengthSquared(const Vec3& inValue)
                {
                    return inValue.dot(inValue);
                }

                bool buildAxisBasis(const Vec3& inDirection, Vec3& outRight, Vec3& outUp)
                {
                    const float length = std::sqrt(lengthSquared(inDirection));
                    if (length <= 1e-6f)
                    {
                        return false;
                    }

                    const Vec3 axis = inDirection / length;
                    const Vec3 hint = std::abs(axis.z) < 0.999f ? Vec3::Up() : Vec3::Right();

                    outRight                = hint.cross(axis);
                    const float rightLength = std::sqrt(lengthSquared(outRight));
                    if (rightLength <= 1e-6f)
                    {
                        return false;
                    }

                    outRight = outRight / rightLength;
                    outUp    = axis.cross(outRight);

                    return true;
                }

            void appendTriangle(
                Vertex::List& outVertices,
                const Vec3&   inA,
                const Vec3&   inB,
                const Vec3&   inC,
                const Vec4&   inColor
            )
            {
                const Vec3  edgeB  = inB - inA;
                const Vec3  edgeC  = inC - inA;
                Vec3        normal = edgeB.cross(edgeC);
                const float length = std::sqrt(lengthSquared(normal));
                if (length > 1e-8f)
                {
                    normal = normal / length;
                }

                Vertex vertex;
                vertex.color  = inColor;
                vertex.normal = normal;

                vertex.position = inA;
                outVertices.push_back(vertex);

                vertex.position = inB;
                outVertices.push_back(vertex);

                vertex.position = inC;
                outVertices.push_back(vertex);
            }

            void appendVertex(
                Vertex::List& outVertices, const Vec3& inPosition, const Vec3& inNormal, const Vec4& inColor
            )
            {
                Vertex vertex;
                vertex.position = inPosition;
                vertex.normal   = inNormal;
                vertex.color    = inColor;
                outVertices.push_back(vertex);
            }
            }

            void appendSegment(Vertex::List& outVertices, const Vec3& inStart, const Vec3& inEnd, const Vec4& inColor)
            {
                Vertex start;
                start.position = inStart;
                start.color    = inColor;

                Vertex end;
                end.position = inEnd;
                end.color    = inColor;

                outVertices.push_back(start);
                outVertices.push_back(end);
            }

            void appendAxes(Vertex::List& outVertices, const Vec3& inCenter, float inSize, const Vec4& inColor)
            {
                appendSegment(
                    outVertices, inCenter - Vec3(inSize, 0.0f, 0.0f), inCenter + Vec3(inSize, 0.0f, 0.0f), inColor
                );
                appendSegment(
                    outVertices, inCenter - Vec3(0.0f, inSize, 0.0f), inCenter + Vec3(0.0f, inSize, 0.0f), inColor
                );
                appendSegment(
                    outVertices, inCenter - Vec3(0.0f, 0.0f, inSize), inCenter + Vec3(0.0f, 0.0f, inSize), inColor
                );
            }

            void appendBone(
                Vertex::List& outVertices,
                const Vec3&   inStart,
                const Vec3&   inEnd,
                float         inRadius,
                const Vec4&   inColor
            )
            {
                appendSegment(outVertices, inStart, inEnd, inColor);

                if (inRadius > 1e-6f)
                {
                    appendSphere(outVertices, inEnd, inRadius, inColor);
                }
            }

            void appendSphere(Vertex::List& outVertices, const Vec3& inCenter, float inRadius, const Vec4& inColor)
            {
                constexpr int kStacks = 8;
                constexpr int kSlices = 12;

                Vec3 rings[kStacks + 1][kSlices + 1];
                Vec3 normals[kStacks + 1][kSlices + 1];

                for (int stack = 0; stack <= kStacks; ++stack)
                {
                    const float theta = kPi * static_cast<float>(stack) / static_cast<float>(kStacks);
                    const float y     = std::cos(theta);
                    const float ring  = std::sin(theta);

                    for (int slice = 0; slice <= kSlices; ++slice)
                    {
                        const float phi = 2.0f * kPi * static_cast<float>(slice) / static_cast<float>(kSlices);
                        const Vec3  normal(ring * std::cos(phi), y, ring * std::sin(phi));

                        normals[stack][slice] = normal;
                        rings[stack][slice]   = inCenter + normal * inRadius;
                    }
                }

                for (int stack = 0; stack < kStacks; ++stack)
                {
                    for (int slice = 0; slice < kSlices; ++slice)
                    {
                        const Vec3& a = rings[stack][slice];
                        const Vec3& b = rings[stack + 1][slice];
                        const Vec3& c = rings[stack][slice + 1];
                        const Vec3& d = rings[stack + 1][slice + 1];

                        appendVertex(outVertices, a, normals[stack][slice], inColor);
                        appendVertex(outVertices, b, normals[stack + 1][slice], inColor);
                        appendVertex(outVertices, c, normals[stack][slice + 1], inColor);

                        appendVertex(outVertices, c, normals[stack][slice + 1], inColor);
                        appendVertex(outVertices, b, normals[stack + 1][slice], inColor);
                        appendVertex(outVertices, d, normals[stack + 1][slice + 1], inColor);
                    }
                }
            }

            void appendBounds(Vertex::List& outVertices, const Bounds3D& inBounds, const Vec4& inColor)
            {
                const Vertex::List& corners = inBounds.getCorners();
                if (corners.size() < 8)
                {
                    return;
                }

                static constexpr int edges[12][2] = {
                    {0, 1},
                    {1, 2},
                    {2, 3},
                    {3, 0},
                    {4, 5},
                    {5, 6},
                    {6, 7},
                    {7, 4},
                    {0, 4},
                    {1, 5},
                    {2, 6},
                    {3, 7}
                };

                for (const int (&edge)[2] : edges)
                {
                    appendSegment(outVertices, corners[edge[0]].position, corners[edge[1]].position, inColor);
                }
            }

            void appendRing(
                Vertex::List& outVertices,
                const Vec3&   inCenter,
                const Vec3&   inRight,
                const Vec3&   inUp,
                float         inRadius,
                const Vec4&   inColor
            )
            {
                if (inRadius <= 0.0f)
                {
                    return;
                }

                for (int i = 0; i < kRingSegments; ++i)
                {
                    const float angle0 = (kPi * 2.0f * static_cast<float>(i)) / static_cast<float>(kRingSegments);
                    const float angle1 = (kPi * 2.0f * static_cast<float>(i + 1)) / static_cast<float>(kRingSegments);

                    const Vec3 start =
                        inCenter + inRight * (std::cos(angle0) * inRadius) + inUp * (std::sin(angle0) * inRadius);
                    const Vec3 end =
                        inCenter + inRight * (std::cos(angle1) * inRadius) + inUp * (std::sin(angle1) * inRadius);

                    appendSegment(outVertices, start, end, inColor);
                }
            }

            void appendRadial(
                Vertex::List& outVertices,
                const Vec3&   inOrigin,
                const Vec3&   inDestination,
                float         inStartRadius,
                float         inEndRadius,
                const Vec4&   inColor
            )
            {
                const Vec3  direction = inDestination - inOrigin;
                const float length    = std::sqrt(lengthSquared(direction));
                if (length <= 0.0f)
                {
                    return;
                }

                Vec3 right;
                Vec3 up;
                if (!buildAxisBasis(direction / length, right, up))
                {
                    return;
                }

                appendSegment(outVertices, inOrigin, inDestination, inColor);

                for (int i = 0; i <= kSweepRings; ++i)
                {
                    const float fraction = static_cast<float>(i) / static_cast<float>(kSweepRings);
                    const float radius   = inStartRadius + (inEndRadius - inStartRadius) * fraction;
                    const Vec3  center   = inOrigin + direction * fraction;

                    appendRing(outVertices, center, right, up, radius, inColor);
                }

                for (int i = 0; i < 4; ++i)
                {
                    const float angle  = (kPi * 2.0f * static_cast<float>(i)) / 4.0f;
                    const Vec3  offset = right * std::cos(angle) + up * std::sin(angle);

                    appendSegment(
                        outVertices,
                        inOrigin + offset * inStartRadius,
                        inDestination + offset * inEndRadius,
                        inColor
                    );
                }
            }

            void appendRectangle(
                Vertex::List& outVertices,
                const Vec3&   inOrigin,
                const Vec3&   inDestination,
                float         inHalfExtentX,
                float         inHalfExtentY,
                const Vec4&   inColor
            )
            {
                const Vec3  direction = inDestination - inOrigin;
                const float length    = std::sqrt(lengthSquared(direction));
                if (length <= 0.0f)
                {
                    return;
                }

                Vec3 right;
                Vec3 up;
                if (!buildAxisBasis(direction / length, right, up))
                {
                    return;
                }

                const float hx = std::max(0.0f, inHalfExtentX);
                const float hy = std::max(0.0f, inHalfExtentY);

                const Vec3 startCorners[4] = {
                    inOrigin + right * hx + up * hy,
                    inOrigin + right * hx - up * hy,
                    inOrigin - right * hx - up * hy,
                    inOrigin - right * hx + up * hy
                };
                const Vec3 endCorners[4] = {
                    inDestination + right * hx + up * hy,
                    inDestination + right * hx - up * hy,
                    inDestination - right * hx - up * hy,
                    inDestination - right * hx + up * hy
                };

                for (int i = 0; i < 4; ++i)
                {
                    const int next = (i + 1) % 4;

                    appendSegment(outVertices, startCorners[i], startCorners[next], inColor);
                    appendSegment(outVertices, endCorners[i], endCorners[next], inColor);
                    appendSegment(outVertices, startCorners[i], endCorners[i], inColor);
                }
            }

            void appendBox(
                Vertex::List&    outVertices,
                const Vec3&      inCenter,
                const QuatFloat& inRotation,
                const Vec3&      inHalfExtents,
                const Vec4&      inColor
            )
            {
                const Vec3 half = Vec3(
                    std::max(0.05f, inHalfExtents.x),
                    std::max(0.05f, inHalfExtents.y),
                    std::max(0.05f, inHalfExtents.z)
                );

                auto toWorld = [&](const Vec3& inLocal) { return inCenter + inRotation * inLocal; };

                const Vec3 corners[8] = {
                    toWorld(Vec3(-half.x, -half.y, -half.z)),
                    toWorld(Vec3(half.x, -half.y, -half.z)),
                    toWorld(Vec3(half.x, half.y, -half.z)),
                    toWorld(Vec3(-half.x, half.y, -half.z)),
                    toWorld(Vec3(-half.x, -half.y, half.z)),
                    toWorld(Vec3(half.x, -half.y, half.z)),
                    toWorld(Vec3(half.x, half.y, half.z)),
                    toWorld(Vec3(-half.x, half.y, half.z))
                };

                static constexpr int edges[12][2] = {
                    {0, 1},
                    {1, 2},
                    {2, 3},
                    {3, 0},
                    {4, 5},
                    {5, 6},
                    {6, 7},
                    {7, 4},
                    {0, 4},
                    {1, 5},
                    {2, 6},
                    {3, 7}
                };

                for (const int (&edge)[2] : edges)
                {
                    appendSegment(outVertices, corners[edge[0]], corners[edge[1]], inColor);
                }
            }

            void appendCapsule(
                Vertex::List&    outVertices,
                const Vec3&      inCenter,
                const QuatFloat& inRotation,
                float            inRadius,
                float            inHalfHeight,
                const Vec4&      inColor
            )
            {
                const float radius     = std::max(0.05f, inRadius);
                const float halfHeight = std::max(0.0f, inHalfHeight);
                const Vec3  axis       = inRotation * Vec3(0.0f, 0.0f, 1.0f);
                const Vec3  right      = inRotation * Vec3(1.0f, 0.0f, 0.0f);
                const Vec3  up         = inRotation * Vec3(0.0f, 1.0f, 0.0f);
                const Vec3  bottom     = inCenter - axis * halfHeight;
                const Vec3  top        = inCenter + axis * halfHeight;

                appendRing(outVertices, bottom, right, up, radius, inColor);
                appendRing(outVertices, top, right, up, radius, inColor);

                for (int i = 0; i < 4; ++i)
                {
                    const float angle  = (kPi * 2.0f * static_cast<float>(i)) / 4.0f;
                    const Vec3  offset = right * std::cos(angle) + up * std::sin(angle);

                    appendSegment(outVertices, bottom + offset * radius, top + offset * radius, inColor);

                    Vec3 previous = bottom + offset * radius;
                    for (int step = 1; step <= 4; ++step)
                    {
                        const float latitude   = (kPi * 0.5f * static_cast<float>(step)) / 4.0f;
                        const float ringRadius = std::cos(latitude) * radius;
                        const float height     = std::sin(latitude) * radius;
                        const Vec3  point      = bottom - axis * height + offset * ringRadius;

                        appendSegment(outVertices, previous, point, inColor);
                        previous = point;
                    }

                    previous = top + offset * radius;
                    for (int step = 1; step <= 4; ++step)
                    {
                        const float latitude   = (kPi * 0.5f * static_cast<float>(step)) / 4.0f;
                        const float ringRadius = std::cos(latitude) * radius;
                        const float height     = std::sin(latitude) * radius;
                        const Vec3  point      = top + axis * height + offset * ringRadius;

                        appendSegment(outVertices, previous, point, inColor);
                        previous = point;
                    }
                }
            }
        }
    }
}
