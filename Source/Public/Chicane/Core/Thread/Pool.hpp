#pragma once

#include "Chicane/Base.hpp"

namespace Chicane
{
    namespace Thread
    {
        class Instance;

        class Pool
	    {
	    public:
            std::uint32_t getSize();
            std::uint32_t getMaxSize();

            bool has(const std::string& inId);
            Instance* add(const std::string& inId);
            Instance* get(const std::string& inId);
            void clear();

            void addJob(const std::string& inId, std::function<void()> inFunction);
            void wait();

        private:
            std::unordered_map<std::string, std::unique_ptr<Instance>> m_threads; 
	    };
    }
}