#include "Chicane/Box/Asset/Preview.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <vector>

#include "Chicane/Box/Asset/Header.hpp"

#include "Chicane/Core/Base64.hpp"
#include "Chicane/Core/Xml.hpp"

namespace Chicane
{
    namespace Box
    {
        void writeRgba(unsigned char* outPixel, const Color::Rgba& inColor)
        {
            outPixel[0] = inColor.r;
            outPixel[1] = inColor.g;
            outPixel[2] = inColor.b;
            outPixel[3] = inColor.a;
        }

        void writeRgba(std::vector<unsigned char>& outPixels, std::size_t inIndex, const Color::Rgba& inColor)
        {
            writeRgba(outPixels.data() + (inIndex * static_cast<std::size_t>(AssetPreview::CHANNELS)), inColor);
        }

        void writeRgba(std::vector<unsigned char>& outPixels, std::size_t inIndex, const unsigned char* inColor)
        {
            std::memcpy(
                outPixels.data() + (inIndex * static_cast<std::size_t>(AssetPreview::CHANNELS)),
                inColor,
                static_cast<std::size_t>(AssetPreview::CHANNELS)
            );
        }

        Color::Rgba scaledRgb(const Color::Rgba& inColor, float inScale)
        {
            auto channel = [inScale](std::uint8_t inValue)
            {
                return static_cast<std::uint8_t>(
                    std::clamp(static_cast<float>(inValue) * inScale, 0.0f, AssetPreview::CHANNEL_MAX)
                );
            };

            return Color::Rgba(channel(inColor.r), channel(inColor.g), channel(inColor.b), inColor.a);
        }

        void fillBackground(std::vector<unsigned char>& outPixels)
        {
            const std::size_t pixelCount = outPixels.size() / static_cast<std::size_t>(AssetPreview::CHANNELS);
            for (std::size_t i = 0; i < pixelCount; i++)
            {
                writeRgba(outPixels, i, AssetPreview::BACKGROUND_COLOR);
            }
        }

        static std::unique_ptr<AssetPreview> parsePreviewNode(
            const pugi::xml_node& inNode, const FileSystem::Path& inAsset
        )
        {
            if (inNode.empty() || !String(inNode.name()).equals(AssetPreview::TAG))
            {
                return nullptr;
            }

            const int width  = Xml::getAttribute(AssetPreview::WIDTH_ATTRIBUTE_NAME, inNode).as_int();
            const int height = Xml::getAttribute(AssetPreview::HEIGHT_ATTRIBUTE_NAME, inNode).as_int();
            if (width <= 0 || height <= 0)
            {
                return nullptr;
            }

            const Image::Raw encoded = Base64::decodeToUnsigned(inNode.text().as_string());
            if (encoded.empty())
            {
                return nullptr;
            }

            Image::Instance image;
            const bool      isPng =
                encoded.size() >= AssetPreview::PNG_HEADER_SIZE &&
                std::memcmp(encoded.data(), AssetPreview::PNG_SIGNATURE, sizeof(AssetPreview::PNG_SIGNATURE)) == 0;
            if (isPng)
            {
                try
                {
                    image = std::make_shared<Image>(encoded, ImageVendor::Png);
                }
                catch (...)
                {
                    return nullptr;
                }
            }
            else
            {
                const std::size_t bytes = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) *
                                          static_cast<std::size_t>(AssetPreview::CHANNELS);
                if (encoded.size() < bytes)
                {
                    return nullptr;
                }

                image = std::make_shared<Image>(
                    encoded.data(),
                    width,
                    height,
                    AssetPreview::CHANNELS,
                    AssetPreview::CHANNELS
                );
            }

            if (!image || image->getPixels() == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->type =
                AssetHeader::getTypeFromTag(Xml::getAttribute(AssetPreview::TYPE_ATTRIBUTE_NAME, inNode).as_string());
            result->image = image;

            return result;
        }

        String AssetPreview::textureIdOf(const FileSystem::Path& inAsset)
        {
            String result = TEXTURE_PREFIX;
            result.append(inAsset.lexicallyNormal().toString());

            return result;
        }

        std::unique_ptr<AssetPreview> AssetPreview::create(
            const FileSystem::Path& inAsset, AssetType inType, const Image& inImage
        )
        {
            if (inImage.getPixels() == nullptr || inImage.getWidth() <= 0 || inImage.getHeight() <= 0)
            {
                return nullptr;
            }

            std::vector<unsigned char> pixels(
                static_cast<std::size_t>(SIZE) * static_cast<std::size_t>(SIZE) * static_cast<std::size_t>(CHANNELS)
            );
            inImage.blit(pixels.data(), SIZE, SIZE, 0);

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->type                         = inType;
            result->image = std::make_shared<Image>(pixels.data(), SIZE, SIZE, CHANNELS, CHANNELS);

            return result;
        }

        void previewCamera(Vec3& outViewDir, Vec3& outRight, Vec3& outUp, Vec3& outLight)
        {
            outViewDir = AssetPreview::VIEW_DIRECTION.normalize();
            outRight   = outViewDir.cross(Vec3::Up());
            if (outRight.dot(outRight) < AssetPreview::NORMAL_EPSILON)
            {
                outRight = Vec3::Right();
            }

            outRight = outRight.normalize();
            outUp    = outRight.cross(outViewDir).normalize();
            outLight = AssetPreview::LIGHT_DIRECTION.normalize();
        }

        void sampleCubemap(const std::vector<Image::Instance>& inFaces, const Vec3& inDirection, unsigned char* outRgba)
        {
            writeRgba(outRgba, AssetPreview::BACKGROUND_COLOR);

            if (inFaces.empty())
            {
                return;
            }

            Vec3 direction = inDirection;
            if (direction.dot(direction) < AssetPreview::EXTENT_EPSILON)
            {
                return;
            }

            direction = direction.normalize();

            const float absX = std::fabs(direction.x);
            const float absY = std::fabs(direction.y);
            const float absZ = std::fabs(direction.z);

            int   face = 0;
            float sc   = 0.0f;
            float tc   = 0.0f;
            float ma   = absX;

            if (absX >= absY && absX >= absZ)
            {
                face = direction.x >= 0.0f ? 0 : 1;
                ma   = absX;
                sc   = direction.x >= 0.0f ? -direction.z : direction.z;
                tc   = -direction.y;
            }
            else if (absY >= absZ)
            {
                face = direction.y >= 0.0f ? 2 : 3;
                ma   = absY;
                sc   = direction.x;
                tc   = direction.y >= 0.0f ? direction.z : -direction.z;
            }
            else
            {
                face = direction.z >= 0.0f ? 4 : 5;
                ma   = absZ;
                sc   = direction.z >= 0.0f ? direction.x : -direction.x;
                tc   = -direction.y;
            }

            const Image::Instance& image = inFaces.at(std::min(static_cast<std::size_t>(face), inFaces.size() - 1));
            if (!image || image->getPixels() == nullptr || image->getWidth() <= 0 || image->getHeight() <= 0)
            {
                return;
            }

            const int           width    = image->getWidth();
            const int           height   = image->getHeight();
            const int           channels = std::max(1, image->getChannel());
            const float         u        = ((sc / ma) + 1.0f) * AssetPreview::HALF;
            const float         v        = ((tc / ma) + 1.0f) * AssetPreview::HALF;
            const int           x = std::clamp(static_cast<int>(u * static_cast<float>(width - 1)), 0, width - 1);
            const int           y = std::clamp(static_cast<int>(v * static_cast<float>(height - 1)), 0, height - 1);
            const Image::Pixels pixels = image->getPixels();
            const std::size_t   offset =
                ((static_cast<std::size_t>(y) * static_cast<std::size_t>(width)) + static_cast<std::size_t>(x)) *
                static_cast<std::size_t>(channels);

            outRgba[0] = pixels[offset];
            outRgba[1] = channels > 1 ? pixels[offset + 1] : pixels[offset];
            outRgba[2] = channels > 2 ? pixels[offset + 2] : pixels[offset];
            outRgba[3] = channels > 3 ? pixels[offset + 3] : AssetPreview::BACKGROUND_COLOR.a;
        }

        void appendUnitCube(Vertex::List& outVertices, Vertex::Indices& outIndices)
        {
            const Vertex::Index       base    = static_cast<Vertex::Index>(outVertices.size());
            const std::array<Vec3, 8> corners = {
                Vec3(-1.0f, -1.0f, -1.0f),
                Vec3(1.0f, -1.0f, -1.0f),
                Vec3(1.0f, 1.0f, -1.0f),
                Vec3(-1.0f, 1.0f, -1.0f),
                Vec3(-1.0f, -1.0f, 1.0f),
                Vec3(1.0f, -1.0f, 1.0f),
                Vec3(1.0f, 1.0f, 1.0f),
                Vec3(-1.0f, 1.0f, 1.0f)
            };

            for (const Vec3& corner : corners)
            {
                Vertex vertex;
                vertex.position = corner;
                outVertices.push_back(vertex);
            }

            const std::array<Vertex::Index, 36> faces = {1, 2, 6, 1, 6, 5, 0, 4, 7, 0, 7, 3, 3, 7, 6, 3, 6, 2,
                                                         0, 1, 5, 0, 5, 4, 4, 5, 6, 4, 6, 7, 0, 3, 2, 0, 2, 1};

            for (const Vertex::Index index : faces)
            {
                outIndices.push_back(base + index);
            }
        }

        std::unique_ptr<AssetPreview> rasterPreview(
            const FileSystem::Path&             inAsset,
            AssetType                           inType,
            Vertex::List                        inVertices,
            Vertex::Indices                     inIndices,
            const std::vector<Image::Instance>& inFaces
        )
        {
            if (inVertices.empty() && !inFaces.empty())
            {
                appendUnitCube(inVertices, inIndices);
            }

            if (inAsset.isEmpty() || inVertices.empty())
            {
                return nullptr;
            }

            if (inIndices.empty())
            {
                inIndices.resize(inVertices.size());
                for (Vertex::Index i = 0; i < static_cast<Vertex::Index>(inVertices.size()); i++)
                {
                    inIndices[i] = i;
                }
            }

            const std::size_t triangleCount = inIndices.size() / 3;
            if (triangleCount == 0)
            {
                return nullptr;
            }

            Vec3 minPosition = inVertices.front().position;
            Vec3 maxPosition = minPosition;
            for (const Vertex& vertex : inVertices)
            {
                minPosition = minPosition.min(vertex.position);
                maxPosition = maxPosition.max(vertex.position);
            }

            const Vec3 center = (minPosition + maxPosition) * AssetPreview::HALF;
            const Vec3 extent = maxPosition - minPosition;
            if (extent.dot(extent) <= AssetPreview::EXTENT_EPSILON)
            {
                return nullptr;
            }

            Vec3 viewDir;
            Vec3 right;
            Vec3 up;
            Vec3 light;
            previewCamera(viewDir, right, up, light);

            Vec2 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
            Vec2 max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

            std::vector<Vec3> projected(inVertices.size());
            for (std::size_t i = 0; i < inVertices.size(); i++)
            {
                const Vec3 offset = inVertices.at(i).position - center;
                projected[i].x    = offset.dot(right);
                projected[i].y    = offset.dot(up);
                projected[i].z    = offset.dot(viewDir);
                min.x             = std::min(min.x, projected[i].x);
                max.x             = std::max(max.x, projected[i].x);
                min.y             = std::min(min.y, projected[i].y);
                max.y             = std::max(max.y, projected[i].y);
            }

            const Vec2 span(
                std::max(max.x - min.x, AssetPreview::AREA_EPSILON),
                std::max(max.y - min.y, AssetPreview::AREA_EPSILON)
            );
            const float scale =
                (static_cast<float>(AssetPreview::SIZE) * AssetPreview::GEOMETRY_FIT) / std::max(span.x, span.y);
            const Vec2 mid    = (min + max) * Vec2(AssetPreview::HALF);
            const Vec2 origin = Vec2(static_cast<float>(AssetPreview::SIZE) * AssetPreview::HALF) - (mid * scale);

            const std::size_t pixelCount =
                static_cast<std::size_t>(AssetPreview::SIZE) * static_cast<std::size_t>(AssetPreview::SIZE);
            std::vector<unsigned char> pixels(pixelCount * static_cast<std::size_t>(AssetPreview::CHANNELS), 0);
            std::vector<float>         depth(pixelCount, std::numeric_limits<float>::max());

            fillBackground(pixels);

            auto edge = [](const Vec2& inA, const Vec2& inB, const Vec2& inC)
            { return ((inC.x - inA.x) * (inB.y - inA.y)) - ((inC.y - inA.y) * (inB.x - inA.x)); };

            for (std::size_t triangle = 0; triangle < triangleCount; triangle++)
            {
                const Vertex::Index i0 = inIndices.at((triangle * 3) + 0);
                const Vertex::Index i1 = inIndices.at((triangle * 3) + 1);
                const Vertex::Index i2 = inIndices.at((triangle * 3) + 2);
                if (i0 >= inVertices.size() || i1 >= inVertices.size() || i2 >= inVertices.size())
                {
                    continue;
                }

                const Vec3& a = projected.at(i0);
                const Vec3& b = projected.at(i1);
                const Vec3& c = projected.at(i2);

                const Vec2  screenA((a.x * scale) + origin.x, origin.y + (a.y * scale));
                const Vec2  screenB((b.x * scale) + origin.x, origin.y + (b.y * scale));
                const Vec2  screenC((c.x * scale) + origin.x, origin.y + (c.y * scale));
                const float area = edge(screenA, screenB, screenC);
                if (std::fabs(area) < AssetPreview::AREA_EPSILON)
                {
                    continue;
                }

                Vec3 normal = inVertices.at(i0).normal + inVertices.at(i1).normal + inVertices.at(i2).normal;
                if (normal.dot(normal) < AssetPreview::NORMAL_EPSILON)
                {
                    const Vec3 e0 = inVertices.at(i1).position - inVertices.at(i0).position;
                    const Vec3 e1 = inVertices.at(i2).position - inVertices.at(i0).position;
                    normal        = e0.cross(e1);
                }

                if (normal.dot(normal) < AssetPreview::NORMAL_EPSILON)
                {
                    continue;
                }

                normal            = normal.normalize();
                const float shade = AssetPreview::SHADE_MIN + (AssetPreview::SHADE_RANGE * std::abs(normal.dot(light)));
                const Color::Rgba clay = scaledRgb(AssetPreview::CLAY_COLOR, shade);

                const int minPx =
                    std::max(0, static_cast<int>(std::floor(std::min({screenA.x, screenB.x, screenC.x}))));
                const int maxPx = std::min(
                    AssetPreview::SIZE - 1,
                    static_cast<int>(std::ceil(std::max({screenA.x, screenB.x, screenC.x})))
                );
                const int minPy =
                    std::max(0, static_cast<int>(std::floor(std::min({screenA.y, screenB.y, screenC.y}))));
                const int maxPy = std::min(
                    AssetPreview::SIZE - 1,
                    static_cast<int>(std::ceil(std::max({screenA.y, screenB.y, screenC.y})))
                );

                for (int py = minPy; py <= maxPy; py++)
                {
                    for (int px = minPx; px <= maxPx; px++)
                    {
                        const Vec2 sample(
                            static_cast<float>(px) + AssetPreview::PIXEL_CENTER,
                            static_cast<float>(py) + AssetPreview::PIXEL_CENTER
                        );
                        const float w0 = edge(screenB, screenC, sample);
                        const float w1 = edge(screenC, screenA, sample);
                        const float w2 = edge(screenA, screenB, sample);
                        if ((w0 * area) < 0.0f || (w1 * area) < 0.0f || (w2 * area) < 0.0f)
                        {
                            continue;
                        }

                        const float       z = ((a.z * w0) + (b.z * w1) + (c.z * w2)) / area;
                        const std::size_t index =
                            (static_cast<std::size_t>(py) * static_cast<std::size_t>(AssetPreview::SIZE)) +
                            static_cast<std::size_t>(px);
                        if (z >= depth.at(index))
                        {
                            continue;
                        }

                        depth[index] = z;

                        if (inFaces.empty())
                        {
                            writeRgba(pixels, index, clay);

                            continue;
                        }

                        const Vec3 position = ((inVertices.at(i0).position * w0) + (inVertices.at(i1).position * w1) +
                                               (inVertices.at(i2).position * w2)) /
                                              area;
                        unsigned char color[AssetPreview::CHANNELS] = {};
                        writeRgba(color, AssetPreview::BACKGROUND_COLOR);
                        sampleCubemap(inFaces, position, color);
                        writeRgba(pixels, index, color);
                    }
                }
            }

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->type                         = inType;
            result->image                        = std::make_shared<Image>(
                pixels.data(),
                AssetPreview::SIZE,
                AssetPreview::SIZE,
                AssetPreview::CHANNELS,
                AssetPreview::CHANNELS
            );

            return result;
        }

        std::unique_ptr<AssetPreview> AssetPreview::createFromGeometry(
            const FileSystem::Path& inAsset, const Vertex::List& inVertices, const Vertex::Indices& inIndices
        )
        {
            return rasterPreview(inAsset, AssetType::Mesh, inVertices, inIndices, {});
        }

        std::unique_ptr<AssetPreview> AssetPreview::createFromSky(
            const FileSystem::Path&             inAsset,
            const Vertex::List&                 inVertices,
            const Vertex::Indices&              inIndices,
            const std::vector<Image::Instance>& inFaces
        )
        {
            return rasterPreview(inAsset, AssetType::Sky, inVertices, inIndices, inFaces);
        }

        std::unique_ptr<AssetPreview> AssetPreview::createFromFont(
            const FileSystem::Path& inAsset, const FontFamily& inFamily, const String& inLabel
        )
        {
            if (inAsset.isEmpty())
            {
                return nullptr;
            }

            String label = inLabel.trim();
            if (label.isEmpty())
            {
                label = inFamily.getFamily();
            }
            if (label.isEmpty())
            {
                label = inFamily.getName();
            }
            if (label.isEmpty())
            {
                label = inAsset.stem().toString();
            }

            auto appendGlyph = [&](std::vector<Vec2>&          outPoints,
                                   std::vector<Vertex::Index>& outIndices,
                                   const FontGlyph&            inGlyph,
                                   float                       inCursor)
            {
                if (inGlyph.vertices.empty())
                {
                    return false;
                }

                const Vertex::Index base = static_cast<Vertex::Index>(outPoints.size());
                for (const Vertex& vertex : inGlyph.vertices)
                {
                    outPoints.push_back({vertex.position.x + inCursor, vertex.position.y});
                }

                if (inGlyph.indices.empty())
                {
                    for (Vertex::Index i = 0; i < static_cast<Vertex::Index>(inGlyph.vertices.size()); i++)
                    {
                        outIndices.push_back(base + i);
                    }
                }
                else
                {
                    for (const Vertex::Index index : inGlyph.indices)
                    {
                        outIndices.push_back(base + index);
                    }
                }

                return true;
            };

            auto layout = [&](const std::vector<char32_t>& inCodes)
            {
                std::vector<Vec2>          points   = {};
                std::vector<Vertex::Index> indices  = {};
                float                      cursor   = 0.0f;
                char32_t                   previous = 0;

                for (char32_t code : inCodes)
                {
                    if (previous != 0)
                    {
                        cursor += inFamily.getKerning(previous, code);
                    }

                    if (!inFamily.hasGlyph(code))
                    {
                        if (code == U' ')
                        {
                            cursor += SPACE_ADVANCE;
                        }

                        previous = code;

                        continue;
                    }

                    const FontGlyph& glyph = inFamily.getGlyph(code);
                    appendGlyph(points, indices, glyph, cursor);
                    cursor += glyph.advance;
                    previous = code;
                }

                return std::make_pair(std::move(points), std::move(indices));
            };

            std::vector<char32_t> codes = label.toUnicode();
            auto                  mesh  = layout(codes);
            if (mesh.first.empty() || mesh.second.size() < 3)
            {
                codes = {'A', 'a'};
                mesh  = layout(codes);
            }

            if (mesh.first.empty() || mesh.second.size() < 3)
            {
                codes.clear();
                for (char32_t code = U'A'; code <= U'Z' && codes.size() < FONT_FALLBACK_COUNT; code++)
                {
                    if (inFamily.hasGlyph(code))
                    {
                        codes.push_back(code);
                    }
                }

                mesh = layout(codes);
            }

            if (mesh.first.empty() || mesh.second.size() < 3)
            {
                codes.clear();
                for (const auto& entry : inFamily.getGlyphs())
                {
                    if (entry.second.vertices.empty())
                    {
                        continue;
                    }

                    codes.push_back(entry.first);
                    if (codes.size() >= FONT_FALLBACK_COUNT)
                    {
                        break;
                    }
                }

                mesh = layout(codes);
            }

            if (mesh.first.empty() || mesh.second.size() < 3)
            {
                return nullptr;
            }

            const std::vector<Vec2>&          points  = mesh.first;
            const std::vector<Vertex::Index>& indices = mesh.second;

            Vec2 min = {points.front().x, points.front().y};
            Vec2 max = {min.x, min.y};
            for (const Vec2& point : points)
            {
                min.x = std::min(min.x, point.x);
                max.x = std::max(max.x, point.x);
                min.y = std::min(min.y, point.y);
                max.y = std::max(max.y, point.y);
            }

            const Vec2  span(std::max(max.x - min.x, AREA_EPSILON), std::max(max.y - min.y, AREA_EPSILON));
            const float scale  = (static_cast<float>(SIZE) * GLYPH_FIT) / std::max(span.x, span.y);
            const Vec2  mid    = (min + max) * Vec2(HALF);
            const Vec2  origin = Vec2(static_cast<float>(SIZE) * HALF) - (mid * scale);

            const std::size_t          pixelCount = static_cast<std::size_t>(SIZE) * static_cast<std::size_t>(SIZE);
            std::vector<unsigned char> pixels(pixelCount * static_cast<std::size_t>(CHANNELS), 0);

            fillBackground(pixels);

            auto edge = [](const Vec2& inA, const Vec2& inB, const Vec2& inC)
            { return ((inC.x - inA.x) * (inB.y - inA.y)) - ((inC.y - inA.y) * (inB.x - inA.x)); };

            const std::size_t triangleCount = indices.size() / 3;
            for (std::size_t triangle = 0; triangle < triangleCount; triangle++)
            {
                const Vertex::Index i0 = indices.at((triangle * 3) + 0);
                const Vertex::Index i1 = indices.at((triangle * 3) + 1);
                const Vertex::Index i2 = indices.at((triangle * 3) + 2);
                if (i0 >= points.size() || i1 >= points.size() || i2 >= points.size())
                {
                    continue;
                }

                const Vec2  screenA((points.at(i0).x * scale) + origin.x, origin.y + (points.at(i0).y * scale));
                const Vec2  screenB((points.at(i1).x * scale) + origin.x, origin.y + (points.at(i1).y * scale));
                const Vec2  screenC((points.at(i2).x * scale) + origin.x, origin.y + (points.at(i2).y * scale));
                const float area = edge(screenA, screenB, screenC);
                if (std::fabs(area) < AREA_EPSILON)
                {
                    continue;
                }

                const int minPx =
                    std::max(0, static_cast<int>(std::floor(std::min({screenA.x, screenB.x, screenC.x}))));
                const int maxPx =
                    std::min(SIZE - 1, static_cast<int>(std::ceil(std::max({screenA.x, screenB.x, screenC.x}))));
                const int minPy =
                    std::max(0, static_cast<int>(std::floor(std::min({screenA.y, screenB.y, screenC.y}))));
                const int maxPy =
                    std::min(SIZE - 1, static_cast<int>(std::ceil(std::max({screenA.y, screenB.y, screenC.y}))));

                for (int py = minPy; py <= maxPy; py++)
                {
                    for (int px = minPx; px <= maxPx; px++)
                    {
                        const Vec2 sample(
                            static_cast<float>(px) + AssetPreview::PIXEL_CENTER,
                            static_cast<float>(py) + AssetPreview::PIXEL_CENTER
                        );
                        const float w0 = edge(screenB, screenC, sample);
                        const float w1 = edge(screenC, screenA, sample);
                        const float w2 = edge(screenA, screenB, sample);
                        if ((w0 * area) < 0.0f || (w1 * area) < 0.0f || (w2 * area) < 0.0f)
                        {
                            continue;
                        }

                        const std::size_t index = (static_cast<std::size_t>(py) * static_cast<std::size_t>(SIZE)) +
                                                  static_cast<std::size_t>(px);
                        writeRgba(pixels, index, FOREGROUND_COLOR);
                    }
                }
            }

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->type                         = AssetType::Font;
            result->image = std::make_shared<Image>(pixels.data(), SIZE, SIZE, CHANNELS, CHANNELS);

            return result;
        }

        std::unique_ptr<AssetPreview> AssetPreview::createFromSound(
            const FileSystem::Path& inAsset, const std::vector<unsigned char>& inData
        )
        {
            if (inData.empty())
            {
                return nullptr;
            }

            auto readU16 = [](const unsigned char* inBytes)
            { return static_cast<std::uint16_t>(inBytes[0] | (static_cast<std::uint16_t>(inBytes[1]) << 8)); };
            auto readU32 = [](const unsigned char* inBytes)
            {
                return static_cast<std::uint32_t>(
                    inBytes[0] | (static_cast<std::uint32_t>(inBytes[1]) << 8) |
                    (static_cast<std::uint32_t>(inBytes[2]) << 16) | (static_cast<std::uint32_t>(inBytes[3]) << 24)
                );
            };

            std::vector<float>   samples;
            const unsigned char* bytes = inData.data();
            const std::size_t    size  = inData.size();

            if (size >= WAV_HEADER_SIZE && bytes[0] == 'R' && bytes[1] == 'I' && bytes[2] == 'F' && bytes[3] == 'F' &&
                bytes[8] == 'W' && bytes[9] == 'A' && bytes[10] == 'V' && bytes[11] == 'E')
            {
                std::uint16_t        format   = WAV_FORMAT_PCM;
                std::uint16_t        channels = 1;
                std::uint16_t        bits     = WAV_BITS_16;
                const unsigned char* data     = nullptr;
                std::size_t          dataSize = 0;
                std::size_t          offset   = WAV_HEADER_SIZE;

                while (offset + WAV_CHUNK_HEADER_SIZE <= size)
                {
                    const char*         chunkId   = reinterpret_cast<const char*>(bytes + offset);
                    const std::uint32_t chunkSize = readU32(bytes + offset + WAV_CHUNK_SIZE_OFFSET);
                    offset += WAV_CHUNK_HEADER_SIZE;

                    if (offset + chunkSize > size)
                    {
                        break;
                    }

                    if (chunkId[0] == 'f' && chunkId[1] == 'm' && chunkId[2] == 't' && chunkId[3] == ' ')
                    {
                        if (chunkSize >= WAV_FMT_MIN_SIZE)
                        {
                            format   = readU16(bytes + offset);
                            channels = std::max<std::uint16_t>(1, readU16(bytes + offset + WAV_FMT_CHANNELS_OFFSET));
                            bits     = readU16(bytes + offset + WAV_FMT_BITS_OFFSET);
                        }
                    }
                    else if (chunkId[0] == 'd' && chunkId[1] == 'a' && chunkId[2] == 't' && chunkId[3] == 'a')
                    {
                        data     = bytes + offset;
                        dataSize = chunkSize;
                    }

                    offset += chunkSize + (chunkSize & 1);
                }

                const int         bytesPerSample = std::max(1, static_cast<int>(bits / WAV_BITS_PER_BYTE));
                const int         frameSize      = std::max(1, static_cast<int>(channels) * bytesPerSample);
                const std::size_t frames = data && frameSize > 0 ? dataSize / static_cast<std::size_t>(frameSize) : 0;
                samples.reserve(frames);

                for (std::size_t frame = 0; frame < frames; frame++)
                {
                    const unsigned char* cursor = data + (frame * static_cast<std::size_t>(frameSize));
                    float                mixed  = 0.0f;

                    for (std::uint16_t channel = 0; channel < channels; channel++)
                    {
                        const unsigned char* sample = cursor + (channel * bytesPerSample);
                        float                value  = 0.0f;

                        if (format == WAV_FORMAT_FLOAT && bits == WAV_BITS_32)
                        {
                            std::uint32_t bitsValue = readU32(sample);
                            float         decoded   = 0.0f;
                            std::memcpy(&decoded, &bitsValue, sizeof(float));
                            value = decoded;
                        }
                        else if (bits == WAV_BITS_8)
                        {
                            value = (static_cast<float>(sample[0]) - WAV_PCM8_BIAS) / WAV_PCM8_BIAS;
                        }
                        else if (bits == WAV_BITS_16)
                        {
                            const std::int16_t decoded =
                                static_cast<std::int16_t>(sample[0] | (static_cast<std::uint16_t>(sample[1]) << 8));
                            value = static_cast<float>(decoded) / WAV_PCM16_SCALE;
                        }
                        else if (bits == WAV_BITS_24)
                        {
                            std::int32_t decoded = sample[0] | (static_cast<std::int32_t>(sample[1]) << 8) |
                                                   (static_cast<std::int32_t>(sample[2]) << 16);
                            if ((decoded & WAV_PCM24_SIGN) != 0)
                            {
                                decoded |= WAV_PCM24_EXTEND;
                            }
                            value = static_cast<float>(decoded) / WAV_PCM24_SCALE;
                        }
                        else if (bits == WAV_BITS_32)
                        {
                            const std::int32_t decoded = static_cast<std::int32_t>(readU32(sample));
                            value                      = static_cast<float>(decoded) / WAV_PCM32_SCALE;
                        }

                        mixed += value;
                    }

                    samples.push_back(mixed / static_cast<float>(channels));
                }
            }

            if (samples.empty())
            {
                samples.reserve(size);
                for (unsigned char byte : inData)
                {
                    samples.push_back((static_cast<float>(byte) - WAV_PCM8_BIAS) / WAV_PCM8_BIAS);
                }
            }

            if (samples.empty())
            {
                return nullptr;
            }

            const std::size_t          pixelCount = static_cast<std::size_t>(SIZE) * static_cast<std::size_t>(SIZE);
            std::vector<unsigned char> pixels(pixelCount * static_cast<std::size_t>(CHANNELS), 0);

            fillBackground(pixels);

            const int mid   = SIZE / 2;
            const int left  = WAVEFORM_MARGIN;
            const int right = SIZE - WAVEFORM_MARGIN;
            for (int x = left; x < right; x++)
            {
                const std::size_t index =
                    (static_cast<std::size_t>(mid) * static_cast<std::size_t>(SIZE)) + static_cast<std::size_t>(x);
                writeRgba(pixels, index, AXIS_COLOR);
            }

            const float       inner     = static_cast<float>(SIZE - (WAVEFORM_MARGIN * 2));
            const int         maxHeight = static_cast<int>(static_cast<float>(SIZE) * WAVEFORM_HEIGHT);
            const std::size_t count     = samples.size();

            for (int x = left; x < right; x++)
            {
                const float       t     = static_cast<float>(x - WAVEFORM_MARGIN) / std::max(inner - 1.0f, 1.0f);
                const std::size_t start = static_cast<std::size_t>(t * static_cast<float>(count));
                const std::size_t end   = std::min(
                    count,
                    static_cast<std::size_t>(
                        ((static_cast<float>(x - WAVEFORM_MARGIN + 1) / std::max(inner, 1.0f)) *
                         static_cast<float>(count)) +
                        1.0f
                    )
                );

                float peak = 0.0f;
                for (std::size_t i = start; i < end; i++)
                {
                    peak = std::max(peak, std::fabs(samples.at(i)));
                }

                const int height = std::max(1, static_cast<int>(peak * static_cast<float>(maxHeight)));
                const int minY   = std::max(0, mid - height);
                const int maxY   = std::min(SIZE - 1, mid + height);

                for (int y = minY; y <= maxY; y++)
                {
                    const std::size_t index =
                        (static_cast<std::size_t>(y) * static_cast<std::size_t>(SIZE)) + static_cast<std::size_t>(x);
                    writeRgba(pixels, index, FOREGROUND_COLOR);
                }
            }

            std::unique_ptr<AssetPreview> result = std::make_unique<AssetPreview>();
            result->path                         = inAsset;
            result->type                         = AssetType::Sound;
            result->image = std::make_shared<Image>(pixels.data(), SIZE, SIZE, CHANNELS, CHANNELS);

            return result;
        }

        bool AssetPreview::write(pugi::xml_node inRoot, AssetType inType, const Image& inImage)
        {
            if (inRoot.empty())
            {
                return false;
            }

            const std::unique_ptr<AssetPreview> preview = create(FileSystem::Path(), inType, inImage);
            if (!preview || !preview->image || !preview->image->getPixels())
            {
                return false;
            }

            pugi::xml_node node = inRoot.child(TAG);
            if (!node.empty())
            {
                inRoot.remove_child(node);
            }

            node = inRoot.prepend_child(TAG);
            if (node.empty())
            {
                return false;
            }

            Xml::addAttribute(node, TYPE_ATTRIBUTE_NAME, AssetHeader::getTypeTag(inType));
            Xml::addAttribute(node, WIDTH_ATTRIBUTE_NAME, String(std::to_string(preview->image->getWidth())));
            Xml::addAttribute(node, HEIGHT_ATTRIBUTE_NAME, String(std::to_string(preview->image->getHeight())));

            const Image::Raw encoded = preview->image->encode();
            if (encoded.empty())
            {
                return false;
            }

            Xml::addText(node, Base64::encode(encoded));

            return true;
        }

        bool AssetPreview::bake(const FileSystem::Path& inAsset, AssetType inType, const Image& inImage)
        {
            if (inAsset.isEmpty() || !FileSystem::exists(inAsset))
            {
                return false;
            }

            try
            {
                pugi::xml_document document = Xml::load(inAsset);
                pugi::xml_node     root     = document.first_child();
                if (root.empty() || !write(root, inType, inImage))
                {
                    return false;
                }

                Xml::save(document, inAsset);

                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        std::unique_ptr<AssetPreview> AssetPreview::read(const FileSystem::Path& inAsset)
        {
            if (inAsset.isEmpty() || !FileSystem::exists(inAsset))
            {
                return nullptr;
            }

            try
            {
                std::ifstream file(inAsset.toStandard(), std::ios::binary);
                if (!file)
                {
                    return nullptr;
                }

                std::string buffer;
                buffer.reserve(PREVIEW_EXTRACT_LIMIT);

                char chunk[PREVIEW_EXTRACT_CHUNK];
                while (file && buffer.size() < PREVIEW_EXTRACT_LIMIT)
                {
                    file.read(chunk, sizeof(chunk));
                    buffer.append(chunk, static_cast<std::size_t>(file.gcount()));

                    const std::size_t start = buffer.find(OPEN_TAG);
                    if (start == std::string::npos)
                    {
                        continue;
                    }

                    const std::size_t end = buffer.find(CLOSE_TAG, start);
                    if (end == std::string::npos)
                    {
                        continue;
                    }

                    const std::size_t  close = end + std::strlen(CLOSE_TAG);
                    pugi::xml_document document;
                    if (!document.load_buffer(
                            buffer.data() + start,
                            close - start,
                            pugi::parse_default | pugi::parse_fragment
                        ))
                    {
                        return nullptr;
                    }

                    return parsePreviewNode(document.first_child(), inAsset);
                }

                return nullptr;
            }
            catch (...)
            {
                return nullptr;
            }
        }

        std::unique_ptr<AssetPreview> AssetPreview::read(const pugi::xml_node& inRoot)
        {
            if (inRoot.empty())
            {
                return nullptr;
            }

            if (String(inRoot.name()).equals(TAG))
            {
                return parsePreviewNode(inRoot, {});
            }

            return parsePreviewNode(inRoot.child(TAG), {});
        }

        String AssetPreview::textureId() const
        {
            return textureIdOf(path);
        }
    }
}
