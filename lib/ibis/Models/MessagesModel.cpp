// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/Models/MessagesModel.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace ibis
{
    namespace models
    {
        struct MessagesModel::Private
        {
            std::shared_ptr<ftk::ObservableList<std::string> > messages;
            std::shared_ptr<ftk::ListObserver<ftk::LogItem> > logObserver;
        };

        void MessagesModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();

            p.messages = ftk::ObservableList<std::string>::create();

            auto logSystem = context->getLogSystem();
            p.logObserver = ftk::ListObserver<ftk::LogItem>::create(
                logSystem->observeLogItems(),
                [this](const std::vector<ftk::LogItem>& items)
                {
                    FTK_P();
                    std::list<std::string> tmp(p.messages->get().begin(), p.messages->get().end());
                    for (const auto& item : items)
                    {
                        if (ftk::LogType::Error == item.type)
                        {
                            tmp.push_back(ftk::Format("ERROR: {0}").arg(item.message));
                        }
                    }
                    while (tmp.size() > 100)
                    {
                        tmp.pop_front();
                    }
                    p.messages->setIfChanged(std::vector<std::string>(tmp.begin(), tmp.end()));
                });
        }

        MessagesModel::MessagesModel() :
            _p(new Private)
        {}

        MessagesModel::~MessagesModel()
        {}

        std::shared_ptr<MessagesModel> MessagesModel::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            auto out = std::shared_ptr<MessagesModel>(new MessagesModel);
            out->_init(context);
            return out;
        }

        std::shared_ptr<ftk::IObservableList<std::string> > MessagesModel::observeMessages() const
        {
            return _p->messages;
        }
    }
}
