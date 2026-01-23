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
            std::shared_ptr<ftk::Observable<std::shared_ptr<Document> > > current;
            std::shared_ptr<ftk::Observable<int> > currentIndex;
        };

        void DocumentModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.context = context;
            p.documents = ftk::ObservableList<std::shared_ptr<Document> >::create();
            p.current = ftk::Observable<std::shared_ptr<Document> >::create();
            p.currentIndex = ftk::Observable<int>::create();
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
            p.current->setIfChanged(document);
            p.currentIndex->setIfChanged(index);
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
                p.current->setIfChanged(!p.documents->isEmpty() ? p.documents->getItem(0) : nullptr);
                p.currentIndex->setIfChanged(0);
            }
        }

        void DocumentModel::closeAll()
        {
            FTK_P();
            p.documents->clear();
            p.current->setIfChanged(nullptr);
            p.currentIndex->setIfChanged(-1);
        }

        const std::shared_ptr<Document>& DocumentModel::getCurrent()
        {
            return _p->current->get();
        }

        std::shared_ptr<ftk::IObservable<std::shared_ptr<Document> > > DocumentModel::observeCurrent() const
        {
            return _p->current;
        }

        std::shared_ptr<ftk::IObservable<int> > DocumentModel::observeCurrentIndex() const
        {
            return _p->currentIndex;
        }

        void DocumentModel::setCurrent(const std::shared_ptr<Document>& value)
        {
            _p->current->setIfChanged(value);
        }

        void DocumentModel::setCurrent(int index)
        {
            FTK_P();
            if (index >= 0 && index < p.documents->getSize())
            {
                p.current->setIfChanged(p.documents->getItem(index));
                p.currentIndex->setIfChanged(index);
            }
        }
    }
}
