#pragma once

#include "Utility.hpp"

namespace Chicane
{
    namespace Box
    {
        class Document
        {
        public:
            Document(const ModelRaw& inData, const String& inBaseDirectory = "", const String& inLabel = "glTF")
            {
                parse(inData, inBaseDirectory, inLabel);
            }

            Document(const FileSystem::Path& inPath)
            {
                if (!FileSystem::exists(inPath))
                {
                    throw std::runtime_error("The glTF file [" + inPath.toString().toStandard() + "] doesn't exist");
                }

                parse(FileSystem::readUnsigned(inPath), inPath.parent().toString(), inPath.toString());
            }

            const tg3_model& get() const { return *m_model.get(); }

            String nodeName(std::int32_t inIndex) const
            {
                const tg3_model& model = get();
                if (inIndex < 0 || static_cast<std::uint32_t>(inIndex) >= model.nodes_count)
                {
                    return String::sprint("Node_%d", inIndex);
                }

                const String name = toString(model.nodes[inIndex].name);
                if (!name.isEmpty())
                {
                    return name;
                }

                return String::sprint("Node_%d", inIndex);
            }

            std::vector<std::int32_t> parents() const
            {
                const tg3_model&          model = get();
                std::vector<std::int32_t> result(model.nodes_count, -1);

                for (std::uint32_t i = 0; i < model.nodes_count; ++i)
                {
                    const tg3_node& node = model.nodes[i];
                    for (std::uint32_t c = 0; c < node.children_count; ++c)
                    {
                        const std::int32_t child = node.children[c];
                        if (child < 0 || static_cast<std::uint32_t>(child) >= model.nodes_count)
                        {
                            continue;
                        }

                        result[static_cast<std::size_t>(child)] = static_cast<std::int32_t>(i);
                    }
                }

                return result;
            }

            const uint8_t* accessorBytes(std::int32_t inAccessor, std::int32_t& outStride) const
            {
                const tg3_model& model = get();
                if (inAccessor < 0 || static_cast<std::uint32_t>(inAccessor) >= model.accessors_count)
                {
                    return nullptr;
                }

                const tg3_accessor& accessor = model.accessors[inAccessor];
                if (accessor.buffer_view < 0 ||
                    static_cast<std::uint32_t>(accessor.buffer_view) >= model.buffer_views_count)
                {
                    return nullptr;
                }

                const tg3_buffer_view& view = model.buffer_views[accessor.buffer_view];
                if (view.buffer < 0 || static_cast<std::uint32_t>(view.buffer) >= model.buffers_count)
                {
                    return nullptr;
                }

                const tg3_buffer& buffer = model.buffers[view.buffer];
                if (!buffer.data.data)
                {
                    return nullptr;
                }

                outStride = tg3_accessor_byte_stride(&accessor, &view);
                if (outStride <= 0)
                {
                    return nullptr;
                }

                const std::uint64_t offset = view.byte_offset + accessor.byte_offset;
                if (offset >= buffer.data.count)
                {
                    return nullptr;
                }

                return buffer.data.data + offset;
            }

            float readComponent(const uint8_t* inBytes, std::int32_t inType, std::int32_t inNormalized) const
            {
                switch (inType)
                {
                case TG3_COMPONENT_TYPE_FLOAT: {
                    float value = 0.0f;
                    std::memcpy(&value, inBytes, sizeof(float));

                    return value;
                }
                case TG3_COMPONENT_TYPE_DOUBLE: {
                    double value = 0.0;
                    std::memcpy(&value, inBytes, sizeof(double));

                    return static_cast<float>(value);
                }
                case TG3_COMPONENT_TYPE_BYTE: {
                    const std::int8_t value = static_cast<std::int8_t>(*inBytes);

                    return inNormalized ? (std::max)(static_cast<float>(value) / 127.0f, -1.0f)
                                        : static_cast<float>(value);
                }
                case TG3_COMPONENT_TYPE_UNSIGNED_BYTE: {
                    return inNormalized ? static_cast<float>(*inBytes) / 255.0f : static_cast<float>(*inBytes);
                }
                case TG3_COMPONENT_TYPE_SHORT: {
                    std::int16_t value = 0;
                    std::memcpy(&value, inBytes, sizeof(std::int16_t));

                    return inNormalized ? (std::max)(static_cast<float>(value) / 32767.0f, -1.0f)
                                        : static_cast<float>(value);
                }
                case TG3_COMPONENT_TYPE_UNSIGNED_SHORT: {
                    std::uint16_t value = 0;
                    std::memcpy(&value, inBytes, sizeof(std::uint16_t));

                    return inNormalized ? static_cast<float>(value) / 65535.0f : static_cast<float>(value);
                }
                case TG3_COMPONENT_TYPE_INT: {
                    std::int32_t value = 0;
                    std::memcpy(&value, inBytes, sizeof(std::int32_t));

                    return static_cast<float>(value);
                }
                case TG3_COMPONENT_TYPE_UNSIGNED_INT: {
                    std::uint32_t value = 0;
                    std::memcpy(&value, inBytes, sizeof(std::uint32_t));

                    return static_cast<float>(value);
                }
                default:
                    return 0.0f;
                }
            }

            bool readFloats(std::int32_t inAccessor, std::vector<float>& outValues, std::int32_t& outComponents) const
            {
                const tg3_model& model = get();
                if (inAccessor < 0 || static_cast<std::uint32_t>(inAccessor) >= model.accessors_count)
                {
                    return false;
                }

                const tg3_accessor& accessor = model.accessors[inAccessor];
                outComponents                = tg3_num_components(accessor.type);
                if (outComponents <= 0)
                {
                    return false;
                }

                std::int32_t   stride = 0;
                const uint8_t* bytes  = accessorBytes(inAccessor, stride);
                if (!bytes)
                {
                    return false;
                }

                const std::int32_t componentSize = tg3_component_size(accessor.component_type);
                if (componentSize <= 0)
                {
                    return false;
                }

                outValues.resize(static_cast<std::size_t>(accessor.count) * static_cast<std::size_t>(outComponents));
                for (std::uint64_t i = 0; i < accessor.count; ++i)
                {
                    const uint8_t* element = bytes + (i * static_cast<std::uint64_t>(stride));
                    for (std::int32_t c = 0; c < outComponents; ++c)
                    {
                        outValues
                            [static_cast<std::size_t>(i) * static_cast<std::size_t>(outComponents) +
                             static_cast<std::size_t>(c)] =
                                readComponent(
                                    element + (c * componentSize),
                                    accessor.component_type,
                                    accessor.normalized
                                );
                    }
                }

                return true;
            }

            bool readIndices(std::int32_t inAccessor, std::vector<std::uint32_t>& outIndices) const
            {
                const tg3_model& model = get();
                if (inAccessor < 0 || static_cast<std::uint32_t>(inAccessor) >= model.accessors_count)
                {
                    return false;
                }

                const tg3_accessor& accessor = model.accessors[inAccessor];
                std::int32_t        stride   = 0;
                const uint8_t*      bytes    = accessorBytes(inAccessor, stride);
                if (!bytes)
                {
                    return false;
                }

                outIndices.resize(static_cast<std::size_t>(accessor.count));
                for (std::uint64_t i = 0; i < accessor.count; ++i)
                {
                    outIndices[static_cast<std::size_t>(i)] = static_cast<std::uint32_t>(
                        readComponent(bytes + (i * static_cast<std::uint64_t>(stride)), accessor.component_type, 0)
                    );
                }

                return true;
            }

            std::int32_t findAttribute(const tg3_primitive& inPrimitive, const char* inName) const
            {
                for (std::uint32_t i = 0; i < inPrimitive.attributes_count; ++i)
                {
                    if (equals(inPrimitive.attributes[i].key, inName))
                    {
                        return inPrimitive.attributes[i].value;
                    }
                }

                return -1;
            }

        private:
            void parse(const ModelRaw& inData, const String& inBaseDirectory, const String& inLabel)
            {
                if (inData.empty())
                {
                    throw std::runtime_error("The glTF [" + inLabel.toStandard() + "] data is empty");
                }

                tg3_parse_options options;
                tg3_parse_options_init(&options);
                options.images_as_is = 1;

                const tg3_error_code error = tinygltf3::parse(
                    m_model,
                    m_errors,
                    inData.data(),
                    static_cast<std::uint64_t>(inData.size()),
                    inBaseDirectory.toChar(),
                    &options
                );

                if (error == TG3_OK && !m_errors.has_error())
                {
                    return;
                }

                String message = "Failed to parse glTF [" + inLabel + "]";
                if (m_errors.count() > 0)
                {
                    const tg3_error_entry* entry = m_errors.entry(0);
                    if (entry && entry->message)
                    {
                        message.append(": ");
                        message.append(entry->message);
                    }
                }

                throw std::runtime_error(message.toStandard());
            }

        private:
            tinygltf3::Model      m_model;
            tinygltf3::ErrorStack m_errors;
        };

    }
}
