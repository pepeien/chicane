#include "Sample/Shooter/Game.hpp"

#include <Chicane/Core/Event/Observable.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Runtime/Instance.hpp>
#include <Chicane/Runtime/Scene.hpp>

#include "Sample/Shooter/Scene.hpp"
#include "Sample/Shooter/UI/View/Home.hpp"

namespace Game
{
    std::uint32_t m_score    = 0;
    std::uint32_t m_maxScore = 0;

    Chicane::EventObservable<std::uint32_t> m_scoreObservable = Chicane::EventObservable<std::uint32_t>();

    static void postScoreEvents()
    {
        if (Chicane::Scene* scene = Chicane::Instance::sInstance().getScene().get())
        {
            scene->send("score", Chicane::String::sSprint("%u / %u", m_score, m_maxScore));

            if (Game::didReachMaxScore())
            {
                scene->send("victory", "");
            }
        }
    }

    void boot()
    {
        Chicane::Instance::sInstance().setScene<Scene>();
        Chicane::Instance::sInstance().setView<HomeView>();
        postScoreEvents();
    }

    std::uint32_t getScore()
    {
        return m_score;
    }

    void incrementScore(std::uint32_t inScore)
    {
        m_score = std::min(m_score + inScore, m_maxScore);

        m_scoreObservable.next(m_score);
        postScoreEvents();
    }

    void decrementScore(std::uint32_t inScore)
    {
        m_score = std::max(m_score - inScore, 0U);

        m_scoreObservable.next(m_score);
        postScoreEvents();
    }

    void watchScore(std::function<void(std::uint32_t)> inNext)
    {
        m_scoreObservable.subscribe(inNext);
    }

    std::uint32_t getMaxScore()
    {
        return m_maxScore;
    }

    bool didReachMaxScore()
    {
        return m_maxScore > 0 && m_score >= m_maxScore;
    }

    void setMaxScore(std::uint32_t inScore)
    {
        m_maxScore = inScore;
        postScoreEvents();
    }
}