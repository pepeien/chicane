#pragma once

#include <atomic>
#include <optional>
#include <utility>
#include <vector>

namespace Chicane
{
    template <typename T>
    class Mailbox
    {
    public:
        Mailbox()
        {
            Node* stub = new Node();
            m_head.store(stub, std::memory_order_relaxed);
            m_tail = stub;
        }

        Mailbox(const Mailbox&)            = delete;
        Mailbox& operator=(const Mailbox&) = delete;
        Mailbox(Mailbox&&)                 = delete;
        Mailbox& operator=(Mailbox&&)      = delete;

        ~Mailbox()
        {
            drain();
            delete m_tail;
            m_tail = nullptr;
        }

    public:
        void push(T inValue)
        {
            Node* node     = new Node(std::move(inValue));
            Node* previous = m_head.exchange(node, std::memory_order_acq_rel);
            previous->next.store(node, std::memory_order_release);
        }

        std::vector<T> drain()
        {
            std::vector<T> result;
            Node*          tail = m_tail;

            for (;;)
            {
                Node* next = tail->next.load(std::memory_order_acquire);
                if (!next)
                {
                    break;
                }

                result.push_back(std::move(*next->value));
                delete tail;
                tail = next;
            }

            m_tail = tail;

            return result;
        }

    private:
        struct Node
        {
        public:
            Node() = default;

            explicit Node(T inValue)
                : value(std::move(inValue))
            {}

        public:
            std::atomic<Node*> next = nullptr;
            std::optional<T>   value;
        };

    private:
        std::atomic<Node*> m_head = nullptr;
        Node*              m_tail = nullptr;
    };
}
