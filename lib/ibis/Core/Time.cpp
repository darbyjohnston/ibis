// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Time.h"

#include <ftk/Core/String.h>

#include <sstream>

namespace opentime
{
    namespace OPENTIME_VERSION
    {
        std::string to_string(const RationalTime& value)
        {
            std::stringstream ss;
            ss << std::fixed << value.value() << "/" << value.rate();
            return ss.str();
        }

        std::string to_string(const TimeRange& value)
        {
            std::stringstream ss;
            ss << std::fixed << value.start_time().value() << "/" <<
                value.duration().value() << "/" <<
                value.duration().rate();
            return ss.str();
        }

        bool from_string(const std::string& s, RationalTime& value)
        {
            bool out = false;
            auto split = ftk::split(s, '/');
            if (2 == split.size())
            {
                value = RationalTime(std::stof(split[0]), std::stof(split[1]));
                out = true;
            }
            return out;
        }

        bool from_string(const std::string& s, TimeRange& value)
        {
            bool out = false;
            auto split = ftk::split(s, '/');
            if (3 == split.size())
            {
                double startTime = 0.0;
                {
                    std::stringstream ss(split[0]);
                    ss >> startTime;
                }
                double duration = 0.0;
                {
                    std::stringstream ss(split[1]);
                    ss >> duration;
                }
                double rate = 0.0;
                {
                    std::stringstream ss(split[2]);
                    ss >> rate;
                }
                value = TimeRange(
                    RationalTime(startTime, rate),
                    RationalTime(duration, rate));
                out = true;
            }
            return out;
        }

        void to_json(nlohmann::json& json, const RationalTime& value)
        {
            json = { value.value(), value.rate() };
        }

        void to_json(nlohmann::json& json, const TimeRange& value)
        {
            json = { value.start_time(), value.duration() };
        }

        void from_json(const nlohmann::json& json, RationalTime& value)
        {
            double v = 0.0;
            double rate = 0.0;
            json.at(0).get_to(v);
            json.at(1).get_to(rate);
            value = RationalTime(v, rate);
        }

        void from_json(const nlohmann::json& json, TimeRange& value)
        {
            RationalTime start;
            RationalTime duration;
            json.at(0).get_to(start);
            json.at(1).get_to(duration);
            value = TimeRange(start, duration);
        }

        std::ostream& operator << (std::ostream& os, const RationalTime& value)
        {
            os << to_string(value);
            return os;
        }

        std::ostream& operator << (std::ostream& os, const TimeRange& value)
        {
            os << to_string(value);
            return os;
        }

        bool cmdLineParse(std::vector<std::string>& args, std::vector<std::string>::iterator& it, RationalTime& value)
        {
            bool out = false;
            if (it != args.end())
            {
                out = from_string(*it, value);
                if (out)
                {
                    it = args.erase(it);
                }
            }
            return out;
        }

        bool cmdLineParse(std::vector<std::string>& args, std::vector<std::string>::iterator& it, TimeRange& value)
        {
            bool out = false;
            if (it != args.end())
            {
                out = from_string(*it, value);
                if (out)
                {
                    it = args.erase(it);
                }
            }
            return out;
        }
    }
}
