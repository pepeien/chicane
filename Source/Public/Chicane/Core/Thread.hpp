#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Thread/Pool.hpp"

namespace Chicane
{
    namespace Thread
    {
        class Instance
	    {
            
	    public:
    		Instance();
    		~Instance();

        public:
    		void wait();

    		void addJob(std::function<void()> inFunction);

        private:
            void queueLoop();

	    private:
    		bool m_idDestroying;

    		std::thread m_worker;
    		std::queue<std::function<void()>> m_jobQueue;
    		std::mutex m_queueMutex;
    		std::condition_variable m_condition;
	    };
    }
}