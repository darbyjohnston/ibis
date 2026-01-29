// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "INode.h"

namespace ibis
{
    namespace render
    {
        NodeConnection::NodeConnection(const std::shared_ptr<INode>& node, int index) :
            node(node),
            index(index)
        {}

        bool NodeConnection::operator == (const NodeConnection& other) const
        {
            return node == other.node && index == other.index;
        }

        bool NodeConnection::operator != (const NodeConnection& other) const
        {
            return !(*this == other);
        }

        void INode::_init(
            const std::shared_ptr<ftk::Context>& context,
            const NodeInfo& info,
            int inputCount,
            int outputCount,
            const NodeAttr& attr)
        {
            _info = info;
            _inputs = ftk::ObservableList<NodeConnection>::create(
                std::vector<NodeConnection>(inputCount));
            _outputs.resize(outputCount);
            _attr = ftk::ObservableMap<std::string, nlohmann::json>::create(attr);
        }

        INode::INode()
        {}

        INode::~INode()
        {}

        const NodeInfo& INode::getInfo() const
        {
            return _info;
        }

        const std::vector<NodeConnection>& INode::getInputs() const
        {
            return _inputs->get();
        }

        std::shared_ptr<ftk::IObservableList<NodeConnection> > INode::observeInputs() const
        {
            return _inputs;
        }

        void INode::setInput(int index, const NodeConnection& connection)
        {
            if (index >= 0 && index < _inputs->getSize())
            {
                _inputs->setItemOnlyIfChanged(index, connection);
            }
        }

        const std::vector<std::shared_ptr<ftk::gl::OffscreenBuffer> >& INode::getOutputs() const
        {
            return _outputs;
        }

        std::vector<std::string> INode::getAttrKeys() const
        {
            return _attr->getKeys();
        }

        nlohmann::json INode::getAttr(const std::string& key) const
        {
            nlohmann::json out;
            if (_attr->hasKey(key))
            {
                out = _attr->getItem(key);
            }
            return out;
        }

        std::shared_ptr<ftk::IObservableMap<std::string, nlohmann::json> > INode::observeAttr() const
        {
            return _attr;
        }

        bool INode::setAttr(const std::string& key, const nlohmann::json& value)
        {
            bool out = false;
            if (_attr->hasKey(key))
            {
                out = _attr->setItemOnlyIfChanged(key, value);
            }
            else
            {
                _attr->setItem(key, value);
                out = true;
            }
            return out;
        }

        void INode::exec(
            const std::shared_ptr<ftk::IRender>& render,
            const OTIO_NS::RationalTime& time)
        {
            for (const auto& input : _inputs->get())
            {
                if (input.node)
                {
                    input.node->exec(render, time);
                }
            }
        }
    }
}