// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/UI/Event.h>

namespace ibis
{
    namespace ui
    {
        //! Node drag and drop data.
        class NodeDragDropData : public ftk::IDragDropData
        {
        public:
            NodeDragDropData(const std::string&);

            virtual ~NodeDragDropData();

            const std::string& getNode() const;

        private:
            std::string _node;
        };
    }
}
