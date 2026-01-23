// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#include <ibis/Models/Document.h>

#include <ftk/Core/ObservableList.h>

#include <filesystem>

namespace ibis
{
    namespace models
    {
        //! Document model.
        class DocumentModel : public std::enable_shared_from_this<DocumentModel>
        {
        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            DocumentModel();

        public:
            virtual ~DocumentModel();

            //! Create a new model.
            static std::shared_ptr<DocumentModel> create(
                const std::shared_ptr<ftk::Context>&);

            //! \name Documents
            ///@{

            const std::vector<std::shared_ptr<Document> >& get();
            std::shared_ptr<ftk::IObservableList<std::shared_ptr<Document> > > observe() const;

            void newDocument();
            void open(const std::filesystem::path&);
            void close(int);
            void closeAll();

            ///@}

            //! \name Current Document
            ///@{

            const std::shared_ptr<Document>& getCurrent();
            std::shared_ptr<ftk::IObservable<std::shared_ptr<Document> > > observeCurrent() const;
            std::shared_ptr<ftk::IObservable<int> > observeCurrentIndex() const;
            void setCurrent(const std::shared_ptr<Document>&);
            void setCurrent(int);

            ///@}

        private:
            FTK_PRIVATE();
        };
    }
}
