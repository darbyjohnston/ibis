// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeDragDrop.h"

namespace ibis
{
    namespace ui
    {
        NodeDragDropData::NodeDragDropData(const std::string& node) :
            _node(node)
        {}

        NodeDragDropData::~NodeDragDropData()
        {}

        const std::string& NodeDragDropData::getNode() const
        {
            return _node;
        }
    }
}
