// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#include "DocumentModel.h"

namespace ibis
{
    namespace models
    {
        struct DocumentModel::Private
        {
            std::weak_ptr<ftk::Context> context;
            std::shared_ptr<ftk::ObservableList<std::shared_ptr<Document> > > documents;
            std::shared_ptr<ftk::Observable<std::shared_ptr<Document> > > currentDocument;
        };

        void DocumentModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.context = context;
            p.documents = ftk::ObservableList<std::shared_ptr<Document> >::create();
            p.currentDocument = ftk::Observable<std::shared_ptr<Document> >::create();
        }

        DocumentModel::DocumentModel() :
            _p(new Private)
        {}

        DocumentModel::~DocumentModel()
        {}

        std::shared_ptr<DocumentModel> DocumentModel::create(const std::shared_ptr<ftk::Context>& context)
        {
            std::shared_ptr<DocumentModel> out(new DocumentModel);
            out->_init(context);
            return out;
        }

        const std::vector<std::shared_ptr<Document> >& DocumentModel::getDocuments()
        {
            return _p->documents->get();
        }

        std::shared_ptr<ftk::IObservableList<std::shared_ptr<Document> > > DocumentModel::observeDocuments() const
        {
            return _p->documents;
        }

        void DocumentModel::newDocument()
        {
            FTK_P();
            auto document = Document::create(p.context.lock());
            p.documents->pushBack(document);
        }

        void DocumentModel::openDocument(const std::filesystem::path&)
        {

        }

        const std::shared_ptr<Document>& DocumentModel::getCurrentDocument()
        {
            return _p->currentDocument->get();
        }

        std::shared_ptr<ftk::IObservable<std::shared_ptr<Document> > > DocumentModel::observeCurrentDocument() const
        {
            return _p->currentDocument;
        }

        void DocumentModel::setCurrentDocument(const std::shared_ptr<Document>& value)
        {
            _p->currentDocument->setIfChanged(value);
        }
    }
}
