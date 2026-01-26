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
            const std::string& id,
            int inputCount,
            int outputCount,
            const NodeAttr& attr)
        {
            _id = id;
            _inputs.resize(inputCount);
            _outputs.resize(outputCount);
            _attr = ftk::ObservableMap<std::string, nlohmann::json>::create(attr);
        }

        INode::INode()
        {}

        INode::~INode()
        {}

        const std::string& INode::getID() const
        {
            return _id;
        }

        const std::vector<NodeConnection>& INode::getInputs() const
        {
            return _inputs;
        }

        void INode::setInput(int index, const NodeConnection& connection)
        {
            if (index >= 0 && index < _inputs.size())
            {
                _inputs[index] = connection;
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
            for (const auto& input : _inputs)
            {
                if (input.node)
                {
                    input.node->exec(render, time);
                }
            }
        }
    }
}