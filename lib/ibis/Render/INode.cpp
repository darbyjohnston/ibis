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
            const NodeInfo& nodeInfo,
            int inputCount,
            int outputCount,
            const NodeAttr& attr)
        {
            _nodeInfo = nodeInfo;
            _inputs = ftk::ObservableList<NodeConnection>::create(
                std::vector<NodeConnection>(inputCount));
            _outputs.resize(outputCount);
            _textureInfo = ftk::ObservableList<ftk::gl::TextureInfo>::create(
                std::vector<ftk::gl::TextureInfo>(outputCount));
            _attr = ftk::ObservableMap<std::string, nlohmann::json>::create(attr);
        }

        INode::INode()
        {}

        INode::~INode()
        {}

        const NodeInfo& INode::getNodeInfo() const
        {
            return _nodeInfo;
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

        ftk::gl::TextureInfo INode::getTextureInfo(int index) const
        {
            return _textureInfo->getItem(index);
        }

        std::shared_ptr<ftk::IObservableList<ftk::gl::TextureInfo> > INode::observeTextureInfo() const
        {
            return _textureInfo;
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

        ftk::M44F INode::_getProjection(const ftk::Size2I& size) const
        {
            return ftk::ortho(
                0.F,
                static_cast<float>(size.w),
                static_cast<float>(size.h),
                0.F,
                -1.F,
                1.F);
        }
    }
}