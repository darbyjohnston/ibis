// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#pragma once

#include <ftk/Core/ObservableList.h>

#include <string>

namespace ftk
{
    class Context;
}

namespace ibis
{
    namespace models
    {
        //! Messages model.
        class MessagesModel : public std::enable_shared_from_this<MessagesModel>
        {
            FTK_NON_COPYABLE(MessagesModel);

        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            MessagesModel();

        public:
            virtual ~MessagesModel();

            //! Create a new model.
            static std::shared_ptr<MessagesModel> create(
                const std::shared_ptr<ftk::Context>&);

            std::shared_ptr<ftk::IObservableList<std::string> > observeMessages() const;

        private:
            FTK_PRIVATE();
        };
    }
}
