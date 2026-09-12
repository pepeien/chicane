#include "Chicane/Renderer/Debug.hpp"

#include <algorithm>
#include <cmath>

#include "Chicane/Core/Math.hpp"
#include "Chicane/Core/Time.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace Debug
        {
            static Trace::List g_traces = {};

            Vertex::List getVertices()
            {
                prune(false);

                Vertex::List result;
                for (const Trace& trace : g_traces)
                {
                    result.insert(result.end(), trace.vertices.begin(), trace.vertices.end());
                }

                return result;
            }

            void push(const Vertex::List& inVertices, float inDuration)
            {
                if (inVertices.empty())
                {
                    return;
                }

                prune(false);

                if (g_traces.size() >= TRACE_CAPACITY)
                {
                    g_traces.erase(g_traces.begin());
                }

                Trace trace;
                trace.vertices = inVertices;

                if (inDuration < 0.0f)
                {
                    trace.bIsPersistant = true;
                }
                else if (inDuration <= 0.0f)
                {
                    trace.bIsForOneFrame = true;
                }
                else
                {
                    trace.expireAt = Time() + Time::fromSeconds(inDuration);
                }

                g_traces.push_back(std::move(trace));
            }

            void prune(bool bInWillExpireOneFrame)
            {
                if (g_traces.empty())
                {
                    return;
                }

                const Time now;

                g_traces.erase(
                    std::remove_if(
                        g_traces.begin(),
                        g_traces.end(),
                        [&](const Trace& inTrace)
                        {
                            if (inTrace.bIsPersistant)
                            {
                                return false;
                            }

                            if (inTrace.bIsForOneFrame)
                            {
                                return bInWillExpireOneFrame;
                            }

                            return now >= inTrace.expireAt;
                        }
                    ),
                    g_traces.end()
                );
            }

            constexpr int SWEEP_RINGS = 4;

            static float lengthSquared(const Vec3& inValue)
            {
                return inValue.dot(inValue);
            }

            static bool buildAxisBasis(const Vec3& inDirection, Vec3& outRight, Vec3& outUp)
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

            static void appendTriangle(
                Vertex::List& outVertices, const Vec3& inA, const Vec3& inB, const Vec3& inC, const Vec4& inColor
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

            static Vec3 normalized(const Vec3& inValue)
            {
                const float length = std::sqrt(lengthSquared(inValue));
                if (length <= 1e-8f)
                {
                    return Vec3::Zero();
                }

                return inValue / length;
            }

            static const Vertex::List& unitSphereTriangles()
            {
                static const Vertex::List vertices = []()
                {
                    constexpr float kGolden = 1.6180339887498948482f;

                    const Vec3 poles[] = {
                        normalized(Vec3(-1.0f, kGolden, 0.0f)),
                        normalized(Vec3(1.0f, kGolden, 0.0f)),
                        normalized(Vec3(-1.0f, -kGolden, 0.0f)),
                        normalized(Vec3(1.0f, -kGolden, 0.0f)),
                        normalized(Vec3(0.0f, -1.0f, kGolden)),
                        normalized(Vec3(0.0f, 1.0f, kGolden)),
                        normalized(Vec3(0.0f, -1.0f, -kGolden)),
                        normalized(Vec3(0.0f, 1.0f, -kGolden)),
                        normalized(Vec3(kGolden, 0.0f, -1.0f)),
                        normalized(Vec3(kGolden, 0.0f, 1.0f)),
                        normalized(Vec3(-kGolden, 0.0f, -1.0f)),
                        normalized(Vec3(-kGolden, 0.0f, 1.0f))
                    };

                    constexpr int faces[][3] = {
                        {0,  11, 5 },
                        {0,  5,  1 },
                        {0,  1,  7 },
                        {0,  7,  10},
                        {0,  10, 11},
                        {1,  5,  9 },
                        {5,  11, 4 },
                        {11, 10, 2 },
                        {10, 7,  6 },
                        {7,  1,  8 },
                        {3,  9,  4 },
                        {3,  4,  2 },
                        {3,  2,  6 },
                        {3,  6,  8 },
                        {3,  8,  9 },
                        {4,  9,  5 },
                        {2,  4,  11},
                        {6,  2,  10},
                        {8,  6,  7 },
                        {9,  8,  1 }
                    };

                    Vertex::List result;
                    result.reserve(80 * 3);

                    auto emit = [&result](const Vec3& inA, const Vec3& inB, const Vec3& inC)
                    {
                        Vertex vertex;
                        vertex.position = inA;
                        vertex.normal   = inA;
                        result.push_back(vertex);

                        vertex.position = inB;
                        vertex.normal   = inB;
                        result.push_back(vertex);

                        vertex.position = inC;
                        vertex.normal   = inC;
                        result.push_back(vertex);
                    };

                    for (const int* face : faces)
                    {
                        const Vec3 a  = poles[face[0]];
                        const Vec3 b  = poles[face[1]];
                        const Vec3 c  = poles[face[2]];
                        const Vec3 ab = normalized(a + b);
                        const Vec3 bc = normalized(b + c);
                        const Vec3 ca = normalized(c + a);

                        emit(a, ab, ca);
                        emit(b, bc, ab);
                        emit(c, ca, bc);
                        emit(ab, bc, ca);
                    }

                    return result;
                }();

                return vertices;
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
                    outVertices,
                    inCenter - Vec3(inSize, 0.0f, 0.0f),
                    inCenter + Vec3(inSize, 0.0f, 0.0f),
                    inColor
                );
                appendSegment(
                    outVertices,
                    inCenter - Vec3(0.0f, inSize, 0.0f),
                    inCenter + Vec3(0.0f, inSize, 0.0f),
                    inColor
                );
                appendSegment(
                    outVertices,
                    inCenter - Vec3(0.0f, 0.0f, inSize),
                    inCenter + Vec3(0.0f, 0.0f, inSize),
                    inColor
                );
            }

            void appendBone(
                Vertex::List& outVertices, const Vec3& inStart, const Vec3& inEnd, float inRadius, const Vec4& inColor
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
                const Vertex::List& unit = unitSphereTriangles();
                outVertices.reserve(outVertices.size() + unit.size());

                Vertex vertex;
                vertex.color = inColor;

                for (const Vertex& source : unit)
                {
                    vertex.position = inCenter + source.position * inRadius;
                    vertex.normal   = source.normal;
                    outVertices.push_back(vertex);
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
                const Vec4&   inColor,
                std::uint32_t inSegmentCount
            )
            {
                if (inRadius <= 0.0f)
                {
                    return;
                }

                const std::uint32_t segments = std::max(3u, inSegmentCount);

                for (std::uint32_t i = 0; i < segments; ++i)
                {
                    const float angle0 = (TWO_PI * static_cast<float>(i)) / static_cast<float>(segments);
                    const float angle1 = (TWO_PI * static_cast<float>(i + 1)) / static_cast<float>(segments);

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
                const Vec4&   inColor,
                std::uint32_t inSegmentCount
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

                const std::uint32_t segments = std::max(3u, inSegmentCount);

                appendSegment(outVertices, inOrigin, inDestination, inColor);

                for (int i = 0; i <= SWEEP_RINGS; ++i)
                {
                    const float fraction = static_cast<float>(i) / static_cast<float>(SWEEP_RINGS);
                    const float radius   = inStartRadius + (inEndRadius - inStartRadius) * fraction;
                    const Vec3  center   = inOrigin + direction * fraction;

                    appendRing(outVertices, center, right, up, radius, inColor, segments);
                }

                for (std::uint32_t i = 0; i < segments; ++i)
                {
                    const float angle  = (TWO_PI * static_cast<float>(i)) / static_cast<float>(segments);
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
                const Vec2&   inHalfExtent,
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

                const float hx = std::max(0.0f, inHalfExtent.x);
                const float hy = std::max(0.0f, inHalfExtent.y);

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
                    const float angle  = (TWO_PI * static_cast<float>(i)) / 4.0f;
                    const Vec3  offset = right * std::cos(angle) + up * std::sin(angle);

                    appendSegment(outVertices, bottom + offset * radius, top + offset * radius, inColor);

                    Vec3 previous = bottom + offset * radius;
                    for (int step = 1; step <= 4; ++step)
                    {
                        const float latitude   = (HALF_PI * static_cast<float>(step)) / 4.0f;
                        const float ringRadius = std::cos(latitude) * radius;
                        const float height     = std::sin(latitude) * radius;
                        const Vec3  point      = bottom - axis * height + offset * ringRadius;

                        appendSegment(outVertices, previous, point, inColor);
                        previous = point;
                    }

                    previous = top + offset * radius;
                    for (int step = 1; step <= 4; ++step)
                    {
                        const float latitude   = (HALF_PI * static_cast<float>(step)) / 4.0f;
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
