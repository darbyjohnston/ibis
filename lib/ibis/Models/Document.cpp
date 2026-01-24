// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "Document.h"

#include "NodeSelectionModel.h"

#include <ibis/Render/Graph.h>

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

        void Document::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.graph = render::Graph::create(context);
            p.commandStack = ftk::CommandStack::create();
            p.selectionModel = NodeSelectionModel::create(context);
            p.path = ftk::Observable<std::filesystem::path>::create("New Document");
            p.timeRange = ftk::Observable<OTIO_NS::TimeRange>::create(OTIO_NS::TimeRange(0.0, 100.0, 24.0));
        }

        Document::Document() :
            _p(new Private)
        {}

        Document::~Document()
        {}

        std::shared_ptr<Document> Document::create(const std::shared_ptr<ftk::Context>&context)
        {
            std::shared_ptr<Document> out(new Document);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Document> Document::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::filesystem::path& path)
        {
            std::shared_ptr<Document> out(new Document);
            out->_init(context);
            out->_p->path->setIfChanged(path);
            return out;
        }

        const std::shared_ptr<render::Graph>& Document::getGraph() const
        {
            return _p->graph;
        }

        const std::shared_ptr<ftk::CommandStack>& Document::getCommandStack() const
        {
            return _p->commandStack;
        }

        const std::shared_ptr<NodeSelectionModel>& Document::getSelectionModel() const
        {
            return _p->selectionModel;
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
    }
}
