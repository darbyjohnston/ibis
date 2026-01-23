// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Render/Graph.h>

#include <ftk/Core/Command.h>

namespace ibis
{
    namespace render
    {
        //! Add a node.
        class AddNodeCommand : public ftk::ICommand
        {
        protected:
            void _init(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                const ftk::V2I&);

            AddNodeCommand();

        public:
            virtual ~AddNodeCommand();

            //! Create a new command.
            static std::shared_ptr<AddNodeCommand> create(
                const std::shared_ptr<Graph>&,
                const std::shared_ptr<INode>&,
                const ftk::V2I&);

            void exec() override;
            void undo() override;

        private:
            FTK_PRIVATE();
        };
    }
}
