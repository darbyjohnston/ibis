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

        void NodeSelectionModel::add(const std::vector<std::shared_ptr<render::INode> >& value)
        {
            FTK_P();
            auto selection = p.selection->get();
            for (const auto& i : value)
            {
                const auto j = std::find(selection.begin(), selection.end(), i);
                if (j == selection.end())
                {
                    selection.push_back(i);
                }
            }
            p.selection->setIfChanged(selection);
        }

        void NodeSelectionModel::remove(const std::vector<std::shared_ptr<render::INode> >& value)
        {
            FTK_P();
            auto selection = p.selection->get();
            auto i = selection.begin();
            while (i != selection.end())
            {
                const auto j = std::find(value.begin(), value.end(), *i);
                if (j != value.end())
                {
                    i = selection.erase(i);
                }
                else
                {
                    ++i;
                }
            }
            p.selection->setIfChanged(selection);
        }

        void NodeSelectionModel::clear()
        {
            _p->selection->clear();
        }
    }
}
