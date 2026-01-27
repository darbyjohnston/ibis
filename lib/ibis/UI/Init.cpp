// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <ibis/UI/Init.h>

#include <ftk/UI/Init.h>
#include <ftk/UI/IconSystem.h>

namespace ibis_resource
{
    extern std::vector<uint8_t> NodeDragDrop;
    extern std::vector<uint8_t> NodeInput;
    extern std::vector<uint8_t> NodeInputConnect;
    extern std::vector<uint8_t> NodeOutput;
    extern std::vector<uint8_t> NodeOutputConnect;
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
            iconSystem->add("NodeInput", ibis_resource::NodeInput);
            iconSystem->add("NodeInputConnect", ibis_resource::NodeInputConnect);
            iconSystem->add("NodeOutput", ibis_resource::NodeOutput);
            iconSystem->add("NodeOutputConnect", ibis_resource::NodeOutputConnect);
            iconSystem->add("View", ibis_resource::View);
            iconSystem->add("ibis", ibis_resource::ibis);
        }
    }
}
