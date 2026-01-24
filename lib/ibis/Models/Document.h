// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ftk/Core/Command.h>
#include <ftk/Core/Context.h>
#include <ftk/Core/Observable.h>

#include <opentimelineio/version.h>

#include <nlohmann/json.hpp>

#include <filesystem>

namespace ibis
{
    namespace render
    {
        class Graph;
        class NodeFactory;
    }

    namespace models
    {
        class NodeSelectionModel;

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

            //! Get the command stack.
            const std::shared_ptr<ftk::CommandStack>& getCommandStack() const;

            //! Get the selection model.
            const std::shared_ptr<NodeSelectionModel>& getSelectionModel() const;

            //! \name Path
            ///@{

            const std::filesystem::path& getPath();
            std::shared_ptr<ftk::IObservable<std::filesystem::path> > observePath() const;

            ///@}

            //! \name Time Range
            ///@{

            const OTIO_NS::TimeRange& getTimeRange();
            std::shared_ptr<ftk::IObservable<OTIO_NS::TimeRange> > observeTimeRange() const;
            void setTimeRange(const OTIO_NS::TimeRange&);

            ///@}

        private:
            FTK_PRIVATE();
        };
    }
}
