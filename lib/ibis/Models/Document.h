// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Command.h>
#include <ftk/Core/Context.h>
#include <ftk/Core/Observable.h>
#include <ftk/Core/Size.h>

#include <opentimelineio/version.h>

#include <nlohmann/json.hpp>

#include <filesystem>

namespace ibis
{
    namespace render
    {
        class Graph;
        class INode;
        class NodeFactory;
    }

    namespace models
    {
        class TimeModel;

        //! Document.
        class Document : public std::enable_shared_from_this<Document>
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::filesystem::path&,
                const nlohmann::json&,
                const std::shared_ptr<render::NodeFactory>&);

            Document();

        public:
            virtual ~Document();

            //! Create a new document.
            static std::shared_ptr<Document> create(
                const std::shared_ptr<ftk::Context>&,
                const std::filesystem::path& = {},
                const nlohmann::json& = {},
                const std::shared_ptr<render::NodeFactory>& = nullptr);

            //! Serialize to JSON.
            nlohmann::json to_json();

            //! Get the graph.
            const std::shared_ptr<render::Graph>& getGraph() const;

            //! \name Path
            ///@{

            const std::filesystem::path& getPath();
            std::shared_ptr<ftk::IObservable<std::filesystem::path> > observePath() const;

            ///@}

            //! \name Time
            ///@{

            const std::shared_ptr<TimeModel>& getTimeModel() const;

            ///@}

            //! \name Editing
            ///@{

            void command(const std::shared_ptr<ftk::ICommand>&);

            void undo();
            void redo();
            std::shared_ptr<ftk::IObservable<bool> > observeHasUndo() const;
            std::shared_ptr<ftk::IObservable<bool> > observeHasRedo() const;

            void deleteSelection();

            ///@}

            //! \name Selection
            ///@{

            const std::vector<std::shared_ptr<render::INode> > getSelection() const;
            std::shared_ptr<ftk::IObservableList<std::shared_ptr<render::INode> > > observeSelection() const;

            void select(const std::vector<std::shared_ptr<render::INode> >&);
            void selectionAdd(const std::vector<std::shared_ptr<render::INode> >&);
            void selectionRemove(const std::vector<std::shared_ptr<render::INode> >&);
            void selectAll();
            void clearSelection();
            void invertSelection();

            ///@}

            //! \name View
            ///@{

            const std::shared_ptr<render::INode> getViewNode() const;
            std::shared_ptr<ftk::IObservable<std::shared_ptr<render::INode> > > observeViewNode() const;
            void setViewNode(const std::shared_ptr<render::INode>&);

            ///@}

            //! \name Canvas Size
            ///@{

            const ftk::Size2I& getCanvasSize();
            std::shared_ptr<ftk::IObservable<ftk::Size2I> > observeCanvasSize() const;
            void setCanvasSize(const ftk::Size2I&);

            ///@}

            //! \name Grid
            ///@{

            bool hasGrid();
            std::shared_ptr<ftk::IObservable<bool> > observeGrid() const;
            void setGrid(bool);

            int getGridSize();
            std::shared_ptr<ftk::IObservable<int> > observeGridSize() const;
            void setGridSize(int);

            ///@}

        private:
            FTK_PRIVATE();
        };
    }
}
