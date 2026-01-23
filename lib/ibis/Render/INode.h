// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Context.h>
#include <ftk/Core/Observable.h>

#include <opentimelineio/version.h>

namespace ibis
{
    namespace render
    {
        //! Base class for nodes.
        class INode : public std::enable_shared_from_this<INode>
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::string&);

            INode();

        public:
            virtual ~INode();

            const std::string& getName() const;

        private:
            FTK_PRIVATE();
        };
    }
}
