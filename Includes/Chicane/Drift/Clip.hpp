#pragma once

#include "Chicane/Core/String.hpp"

#include "Chicane/Drift.hpp"
#include "Chicane/Drift/Loop.hpp"
#include "Chicane/Drift/Track.hpp"

namespace Chicane
{
    namespace Drift
    {
        struct CHICANE_DRIFT Clip
        {
        public:
            Clip();
            Clip(const String& inName);

        public:
            Track* getTrack(const String& inName);
            const Track* getTrack(const String& inName) const;

            void addTrack(const Track& inValue);

        public:
            String      name;
            float       duration;
            Loop        loop;
            int         iterations;
            Track::List tracks;
        };
    }
}
