// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Document.h"

#include "NodeSelectionModel.h"

#include <ibis/Render/Graph.h>
#include <ibis/Render/GraphCmd.h>

namespace ibis
{
    namespace models
    {
        struct Document::Private
        {
            std::shared_ptr<render::Graph> graph;
            std::shared_ptr<ftk::CommandStack> commandStack;
            std::shared_ptr<NodeSelectionModel> selectionModel;
            std::shared_ptr<ftk::Observable<std::filesystem::path> > path;
            std::shared_ptr<ftk::Observable<OTIO_NS::TimeRange> > timeRange;
        };

        void Document::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::filesystem::path& path,
            const nlohmann::json& json,
            const std::shared_ptr<render::NodeFactory>& nodeFactory)
        {
            FTK_P();
            if (!json.empty() && json.contains("Graph"))
            {
                p.graph = render::Graph::create(context, json["Graph"], nodeFactory);
            }
            if (!p.graph)
            {
                p.graph = render::Graph::create(context);
            }
            p.commandStack = ftk::CommandStack::create();
            p.selectionModel = NodeSelectionModel::create(context);
            p.path = ftk::Observable<std::filesystem::path>::create(path);
            p.timeRange = ftk::Observable<OTIO_NS::TimeRange>::create(OTIO_NS::TimeRange(0.0, 100.0, 24.0));
        }

        Document::Document() :
            _p(new Private)
        {}

        Document::~Document()
        {}

        std::shared_ptr<Document> Document::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::filesystem::path& path,
            const nlohmann::json& json,
            const std::shared_ptr<render::NodeFactory>& nodeFactory)
        {
            std::shared_ptr<Document> out(new Document);
            out->_init(context, path, json, nodeFactory);
            return out;
        }

        nlohmann::json Document::to_json()
        {
            FTK_P();
            nlohmann::json out;
            out["Graph"] = p.graph->to_json();
            return out;
        }

        const std::shared_ptr<render::Graph>& Document::getGraph() const
        {
            return _p->graph;
        }

        const std::filesystem::path& Document::getPath()
        {
            return _p->path->get();
        }

        std::shared_ptr<ftk::IObservable<std::filesystem::path> >Document::observePath() const
        {
            return _p->path;
        }

        const OTIO_NS::TimeRange& Document::getTimeRange()
        {
            return _p->timeRange->get();
        }

        std::shared_ptr<ftk::IObservable<OTIO_NS::TimeRange> > Document::observeTimeRange() const
        {
            return _p->timeRange;
        }

        void Document::setTimeRange(const OTIO_NS::TimeRange& value)
        {
            _p->timeRange->setIfChanged(value);
        }

        void Document::command(const std::shared_ptr<ftk::ICommand>& command)
        {
            _p->commandStack->push(command);
        }

        void Document::undo()
        {
            FTK_P();
            p.commandStack->undo();
        }

        void Document::redo()
        {
            FTK_P();
            p.commandStack->redo();
        }

        std::shared_ptr<ftk::IObservable<bool> > Document::observeHasUndo() const
        {
            return _p->commandStack->observeHasUndo();
        }

        std::shared_ptr<ftk::IObservable<bool> > Document::observeHasRedo() const
        {
            return _p->commandStack->observeHasRedo();
        }

        void Document::deleteSelection()
        {
            FTK_P();
            const auto selection = p.selectionModel->get();
            p.selectionModel->clear();
            p.commandStack->push(
                render::RemoveNodesCmd::create(p.graph, selection));
        }

        void Document::select(const std::vector<std::shared_ptr<render::INode> >& value)
        {
            _p->selectionModel->set(value);
        }

        void Document::selectAll()
        {
            FTK_P();
            p.selectionModel->set(p.graph->getNodes());
        }

        void Document::clearSelection()
        {
            _p->selectionModel->clear();
        }

        void Document::invertSelection()
        {
            FTK_P();
            const auto nodes = p.graph->getNodes();
            const auto selection = p.selectionModel->get();
            std::vector<std::shared_ptr<render::INode> > invert;
            for (const auto& node : nodes)
            {
                const auto i = std::find(selection.begin(), selection.end(), node);
                if (i == selection.end())
                {
                    invert.push_back(node);
                }
            }
            p.selectionModel->set(invert);
        }

        const std::vector<std::shared_ptr<render::INode> > Document::getSelection() const
        {
            return _p->selectionModel->get();
        }

        std::shared_ptr<ftk::IObservableList<std::shared_ptr<render::INode> > > Document::observeSelection() const
        {
            return _p->selectionModel->observe();
        }
    }
}
