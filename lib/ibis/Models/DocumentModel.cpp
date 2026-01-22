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
            std::shared_ptr<ftk::Observable<int> > newDocument;
        };

        void DocumentModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.context = context;
            p.documents = ftk::ObservableList<std::shared_ptr<Document> >::create();
            p.newDocument = ftk::Observable<int>::create();
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

        const std::vector<std::shared_ptr<Document> >& DocumentModel::get()
        {
            return _p->documents->get();
        }

        std::shared_ptr<ftk::IObservableList<std::shared_ptr<Document> > > DocumentModel::observe() const
        {
            return _p->documents;
        }

        void DocumentModel::newDocument()
        {
            FTK_P();
            auto document = Document::create(p.context.lock());
            const int index = p.documents->getSize();
            p.documents->pushBack(document);
            p.newDocument->setIfChanged(index);
        }

        std::shared_ptr<ftk::IObservable<int> > DocumentModel::observeNewDocument() const
        {
            return _p->newDocument;
        }

        void DocumentModel::open(const std::filesystem::path&)
        {

        }

        void DocumentModel::close(int index)
        {
            FTK_P();
            if (index >= 0 && index < p.documents->getSize())
            {
                p.documents->removeItem(index);
            }
        }

        void DocumentModel::closeAll()
        {
            _p->documents->clear();
        }
    }
}
