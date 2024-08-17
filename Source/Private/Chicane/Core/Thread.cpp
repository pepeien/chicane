#include "Chicane/Core/Thread.hpp"

namespace Chicane
{
    namespace Thread
    {
        Instance::Instance()
            : m_idDestroying(false),
            m_worker(std::thread(&Instance::queueLoop, this))
        {}

        Instance::~Instance()
    	{
    		if (!m_worker.joinable())
    		{
                return;
    		}

    		wait();

    		m_queueMutex.lock();

    		m_idDestroying = true;

    		m_condition.notify_one();
    		m_queueMutex.unlock();
    		m_worker.join();
    	}

        void Instance::addJob(std::function<void()> inFunction)
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);

    		m_jobQueue.push(std::move(inFunction));
    		m_condition.notify_one();
        }

        void Instance::wait()
    	{
    		std::unique_lock<std::mutex> lock(m_queueMutex);

    		m_condition.wait(lock, [this]() { return m_jobQueue.empty(); });
    	}

        void Instance::queueLoop()
        {
            while (true)
    	    {
    	    	std::function<void()> job;

    	    	{
    	    		std::unique_lock<std::mutex> lock(m_queueMutex);

    	    		m_condition.wait(lock, [this] { return !m_jobQueue.empty() || m_idDestroying; });
    
    	    		if (m_idDestroying)
    	    		{
    	    			break;
    	    		}

    	    		job = m_jobQueue.front();
    	    	}

    	    	job();

    	    	{
    	    		std::lock_guard<std::mutex> lock(m_queueMutex);

    	    		m_jobQueue.pop();

    	    		m_condition.notify_one();
    	    	}
    	    }
        }
    }
}