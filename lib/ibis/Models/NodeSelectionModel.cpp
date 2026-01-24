// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "NodeSelectionModel.h"

namespace ibis
{
    namespace models
    {
        struct NodeSelectionModel::Private
        {
            std::shared_ptr<ftk::ObservableList<std::shared_ptr<render::INode>> > selection;
        };

        void NodeSelectionModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.selection = ftk::ObservableList<std::shared_ptr<render::INode> >::create();
        }

        NodeSelectionModel::NodeSelectionModel() :
            _p(new Private)
        {}

        NodeSelectionModel::~NodeSelectionModel()
        {}

        std::shared_ptr<NodeSelectionModel> NodeSelectionModel::create(const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<NodeSelectionModel> out(new NodeSelectionModel);
            out->_init(context);
            return out;
        }

        const std::vector<std::shared_ptr<render::INode> > NodeSelectionModel::get() const
        {
            return _p->selection->get();
        }

        std::shared_ptr<ftk::IObservableList<std::shared_ptr<render::INode> > > NodeSelectionModel::observe() const
        {
            return _p->selection;
        }

        void NodeSelectionModel::set(const std::vector<std::shared_ptr<render::INode> >& value)
        {
            _p->selection->setIfChanged(value);
        }

        void NodeSelectionModel::add(const std::shared_ptr<render::INode>& value)
        {
            _p->selection->pushBack(value);
        }

        void NodeSelectionModel::remove(const std::shared_ptr<render::INode>& value)
        {
            const size_t i = _p->selection->indexOf(value);
            if (i != ftk::ObservableListInvalidIndex)
            {
                _p->selection->removeItem(i);
            }
        }

        void NodeSelectionModel::clear()
        {
            _p->selection->clear();
        }
    }
}
