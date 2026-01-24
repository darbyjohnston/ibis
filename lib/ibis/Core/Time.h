// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include <opentimelineio/version.h>

#include <nlohmann/json.hpp>

namespace opentime
{
    namespace OPENTIME_VERSION
    {
        std::string to_string(const RationalTime&);
        std::string to_string(const TimeRange&);

        bool from_string(const std::string&, RationalTime&);
        bool from_string(const std::string&, TimeRange&);

        void to_json(nlohmann::json&, const RationalTime&);
        void to_json(nlohmann::json&, const TimeRange&);

        void from_json(const nlohmann::json&, RationalTime&);
        void from_json(const nlohmann::json&, TimeRange&);

        std::ostream& operator << (std::ostream&, const RationalTime&);
        std::ostream& operator << (std::ostream&, const TimeRange&);

        bool cmdLineParse(std::vector<std::string>&, std::vector<std::string>::iterator&, RationalTime&);
        bool cmdLineParse(std::vector<std::string>&, std::vector<std::string>::iterator&, TimeRange&);
    }
}
