#include "Game.hpp"

std::uint32_t m_score = 0;
std::uint32_t m_maxScore = 0;

Chicane::Observable<std::uint32_t> m_scoreObservable = Chicane::Observable<std::uint32_t>();

namespace Game
{
    std::uint32_t getScore()
    {
        return m_score;
    }

    void setScore(std::uint32_t inScore)
    {
        m_score = inScore;

        m_scoreObservable.next(m_score);
    }
 
    bool didReachMaxScore()
    {
        return m_score >= m_maxScore;
    }

    void setMaxScore(std::uint32_t inScore)
    {
        m_maxScore = inScore;
    }

    void watchScore(std::function<void (std::uint32_t)> inNext)
    {
        m_scoreObservable.subscribe(inNext);
    }

    void incrementScore(std::uint32_t inScore)
    {
        m_score += inScore;

        m_scoreObservable.next(m_score);
    }

    void decrementScore(std::uint32_t inScore)
    {
        m_score -= inScore;

        m_scoreObservable.next(m_score);
    }
}