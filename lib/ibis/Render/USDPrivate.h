// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the ibis compositor project.

#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <ftk/Core/Image.h>
#include <ftk/Core/LogSystem.h>
#include <ftk/Core/Path.h>

#include <opentimelineio/version.h>

#include <pxr/usd/usd/stage.h>
#include <pxr/usdImaging/usdImagingGL/engine.h>

#include <future>

namespace ibis
{
    namespace usd
    {
        enum class DrawMode
        {
            Points,
            Wireframe,
            WireframeOnSurface,
            ShadedFlat,
            ShadedSmooth,
            GeomOnly,
            GeomFlat,
            GeomSmooth,

            Count,
            First = Points
        };
        FTK_ENUM(DrawMode);

        struct Options
        {
            int           renderWidth = 1920;
            std::string   cameraName;
            float         complexity = 1.F;
            usd::DrawMode drawMode = usd::DrawMode::ShadedSmooth;
            bool          enableLighting = true;
            bool          sRGB = true;
            size_t        stageCacheCount = 10;

            bool operator == (const Options&) const;
            bool operator != (const Options&) const;
        };

        struct Info
        {
            ftk::ImageInfo     image;
            OTIO_NS::TimeRange timeRange;
        };

        //! USD renderer.
        class Render : public std::enable_shared_from_this<Render>
        {
        protected:
            void _init(const std::shared_ptr<ftk::LogSystem>&);

            Render();

        public:
            ~Render();

            //! Create a new renderer.
            static std::shared_ptr<Render> create(
                const std::shared_ptr<ftk::LogSystem>&);
            
            //! Get information.
            std::future<Info> getInfo(
                int64_t id,
                const ftk::Path& path,
                const Options&);
            
            //! Render an image.
            std::future<std::shared_ptr<ftk::Image> > render(
                int64_t id,
                const ftk::Path& path,
                const OTIO_NS::RationalTime& time,
                const Options&);

            //! Cancel requests.
            void cancelRequests(int64_t id);

        private:
            void _open(
                const std::string&,
                PXR_NS::UsdStageRefPtr&,
                std::shared_ptr<PXR_NS::UsdImagingGLEngine>&);
            void _run();
            void _finish();

            FTK_PRIVATE();
        };
    }
}
