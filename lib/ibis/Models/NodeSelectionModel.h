// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/INode.h>

#include <ftk/Core/ObservableList.h>

namespace ibis
{
    namespace models
    {
        //! Node selection model.
        class NodeSelectionModel : public std::enable_shared_from_this<NodeSelectionModel>
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            NodeSelectionModel();

        public:
            virtual ~NodeSelectionModel();

            //! Create a new model.
            static std::shared_ptr<NodeSelectionModel> create(
                const std::shared_ptr<ftk::Context>&);

            //! Get the selection.
            const std::vector<std::shared_ptr<render::INode> > get() const;

            //! Observe the selection.
            std::shared_ptr<ftk::IObservableList<std::shared_ptr<render::INode> > > observe() const;

            //! Set the selection.
            void set(const std::vector<std::shared_ptr<render::INode> >&);

            //! Add to the selection.
            void add(const std::vector<std::shared_ptr<render::INode> >&);

            //! Remove from the selection.
            void remove(const std::vector<std::shared_ptr<render::INode> >&);
            
            //! Clear the selection.
            void clear();

        private:
            FTK_PRIVATE();
        };
    }
}
