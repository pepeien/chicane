#pragma once

#include "Chicane/Core/Essential.hpp"
#include "Chicane/Core/List/PushStrategy.hpp"

namespace Chicane
{
    template<typename T>
    struct List
    {
    public:
        typedef typename std::vector<T>::iterator Iterator;
        typedef typename std::vector<T>::const_iterator ConstIterator;

    public:
        Iterator begin()
        {
            return m_data.begin();
        }

        const ConstIterator begin() const
        {
            return m_data.begin();
        }

        Iterator end()
        {
            return m_data.end();
        }

        const ConstIterator end() const
        {
            return m_data.end();
        }

        T& at(std::size_t inIndex)
        {
            return m_data.at(calculateIndex(inIndex));
        }

        const T& at(std::size_t inIndex) const
        {
            return m_data.at(calculateIndex(inIndex));
        }

        T& find(std::function<bool (const T& inItem)> inPredicate)
        {
            return std::find_if(m_data.begin(), m_data.end(), inPredicate);
        }

        const T& find(std::function<bool (const T& inItem)> inPredicate) const
        {
            return std::find_if(m_data.begin(), m_data.end(), inPredicate);
        }

        std::size_t findIndex(std::function<bool (const T& inItem)> inPredicate)
        {
            return std::find_if(m_data.begin(), m_data.end(), inPredicate) - m_data.begin();
        }

        std::size_t findIndex(std::function<bool (const T& inItem)> inPredicate) const
        {
            return std::find_if(m_data.begin(), m_data.end(), inPredicate) - m_data.begin();
        }
 
        T& front()
        {
            return m_data.front();
        }

        const T& front() const
        {
            return m_data.front();
        }

        T& back()
        {
            return m_data.back();
        }

        const T& back() const
        {
            return m_data.back();
        }

        void add(T inData, ListPushStrategy inStrategy = ListPushStrategy::Back)
        {
            switch (inStrategy)
            {
            case ListPushStrategy::Back:
                addBack(inData);

                break;

            case ListPushStrategy::Front:
                addFront(inData);

                break;

            default:
                addBack(inData);

                break;
            }
        }

        void addFront(T inData)
        {
            m_data.insert(m_data.begin(), inData);
        }

        void addBack(T inData)
        {
            m_data.push_back(inData);
        }

        void addBefore(std::size_t inIndex, T inData)
        {
            m_data.insert(
                (m_data.begin() + calculateIndex(inIndex)) - 1,
                inData
            );
        }

        void addAfter(std::size_t inIndex, T inData)
        {
            m_data.insert(
                (m_data.begin() + calculateIndex(inIndex)) + 1,
                inData
            );
        }

        void clear()
        {
            m_data.clear();
        }

    private:
        std::size_t calculateIndex(std::size_t inIndex) const
        {
            return std::max(
                0,
                inIndex < 0 ? inIndex - (m_data.size() - 1) : inIndex
            );
        }

    private:
        std::vector<T> m_data = {};
    };
}