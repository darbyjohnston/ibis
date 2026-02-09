// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/Models/MessagesModel.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

namespace ibis
{
    namespace models
    {
        namespace
        {
            const size_t messagesMax = 100;
        }

        struct MessagesModel::Private
        {
            std::shared_ptr<ftk::ObservableList<std::string> > messages;
            std::shared_ptr<ftk::ObservableList<std::string> > log;
            std::shared_ptr<ftk::ListObserver<ftk::LogItem> > logObserver;
        };

        void MessagesModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();

            p.messages = ftk::ObservableList<std::string>::create();

            p.log = ftk::ObservableList<std::string>::create();

            auto logSystem = context->getLogSystem();
            p.logObserver = ftk::ListObserver<ftk::LogItem>::create(
                logSystem->observeLogItems(),
                [this](const std::vector<ftk::LogItem>& items)
                {
                    FTK_P();
                    std::list<std::string> tmp(p.messages->get().begin(), p.messages->get().end());
                    for (const auto& item : items)
                    {
                        switch (item.type)
                        {
                        case ftk::LogType::Warning:
                        case ftk::LogType::Error:
                            for (const auto& line : ftk::split(ftk::getLabel(item), '\n', ftk::SplitOptions::KeepEmpty))
                            {
                                tmp.push_back(line);
                            }
                            break;
                        default: break;
                        }
                        for (const auto& line : ftk::split(ftk::getLabel(item), '\n', ftk::SplitOptions::KeepEmpty))
                        {
                            p.log->pushBack(line);
                        }
                    }
                    while (tmp.size() > messagesMax)
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

        std::shared_ptr<ftk::IObservableList<std::string> > MessagesModel::observeLog() const
        {
            return _p->log;
        }
    }
}
