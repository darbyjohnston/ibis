// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis project.

#include <ibis/UI/Init.h>

#include <ftk/UI/Init.h>
#include <ftk/UI/IconSystem.h>

namespace ibis_resource
{
    extern std::vector<uint8_t> NodeDragDrop;
    extern std::vector<uint8_t> NodePort;
    extern std::vector<uint8_t> NodePortConnected;
    extern std::vector<uint8_t> PlaybackLoop;
    extern std::vector<uint8_t> PlaybackOnce;
    extern std::vector<uint8_t> PlaybackPingPong;
    extern std::vector<uint8_t> View;
    extern std::vector<uint8_t> ibis;
}

namespace ibis
{
    namespace ui
    {
        void init(const std::shared_ptr<ftk::Context>& context)
        {
            ftk::uiInit(context);

            auto iconSystem = context->getSystem<ftk::IconSystem>();
            iconSystem->add("NodeDragDrop", ibis_resource::NodeDragDrop);
            iconSystem->add("NodePort", ibis_resource::NodePort);
            iconSystem->add("NodePortConnected", ibis_resource::NodePortConnected);
            iconSystem->add("PlaybackLoop", ibis_resource::PlaybackLoop);
            iconSystem->add("PlaybackOnce", ibis_resource::PlaybackOnce);
            iconSystem->add("PlaybackPingPong", ibis_resource::PlaybackPingPong);
            iconSystem->add("View", ibis_resource::View);
            iconSystem->add("ibis", ibis_resource::ibis);
        }
    }
}
