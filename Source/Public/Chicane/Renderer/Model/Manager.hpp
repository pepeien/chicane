#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/Model.hpp"
#include "Chicane/Renderer/Vertex.hpp"
#include "Chicane/Renderer/Model/Vendors/Wavefront.hpp"

namespace Chicane
{
    class MeshComponent;

    namespace Model
    {
        enum class Vendor : std::uint8_t
        {
            // N/A
            Undefined,
            // .obj
            Wavefront
        };

        class Manager
        {
        public:
            enum class EventSubject : std::uint8_t
            {
                Load,
                Allocation,
                Use
            };

        public:
            Manager();

        public:
            bool isEmpty() const;
            bool canDraw() const;
            bool isLoaded(const std::string& inId) const;
            bool isAllocated(const std::string& inId) const;
            bool isUsing(const std::string& inId) const;

            const Vec<3, float>& getBounds(const std::string& inId);

            void load(const std::string& inId, const Box::Entry& inEntry);
            void activate(const std::string& inId);
            void deactivate(const std::string& inId);
            void build(
                Buffer::Instance& outVertexBuffer,
                Buffer::Instance& outIndexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );

            std::uint32_t getUseCount(const std::string& inId) const;
            std::uint32_t getFirstInstance(const std::string& inId) const;
            void draw(
                const std::string& inId,
                const vk::CommandBuffer& inCommandBuffer
            );
            void drawAll(const vk::CommandBuffer& inCommandBuffer);

            void watchChanges(
                std::function<void (EventSubject)> inNextCallback,
                std::function<void (const std::string&)> inErrorCallback = nullptr,
                std::function<void ()> inCompleteCallback = nullptr
            );

        private:
            void allocate(const std::string& inId);
            void deallocate(const std::string& inId);
            void rebaseAllocation();

            void initVertexBuffer(
                Buffer::Instance& outVertexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );
            void initIndexBuffer(
                Buffer::Instance& outIndexBuffer,
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::Queue& inQueue,
                const vk::CommandBuffer& inCommandBuffer
            );

            Vec<3, float> getExtent(const std::vector<Vertex::Instance>& inVertices);

        private:
            std::unordered_map<std::string, Model::Instance> m_instanceMap;
            std::unordered_map<std::string, Model::AllocationInfo> m_allocationMap;
            std::vector<std::string> m_usedIds;

            std::vector<Vertex::Instance> m_vertices;
            std::vector<uint32_t> m_indices;

            std::unique_ptr<Observable<EventSubject>> m_observable;
        };
    }
}